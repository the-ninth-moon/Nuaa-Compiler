from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import sys

stack = [0 for i in range(0, 8000)]  # 数据栈 前三个0是主函数的SL DL RA
code = []  # 存放代码

newbase = [0]

def add_code(f, l, a):
    operation = dict()
    operation['F'] = f
    operation['L'] = l
    operation['A'] = a
    code.append(operation)

    # 由当前的basic去找最近的直接外层，B是沿着静态链找的，因为要找最新的
    # 只要保证每一个静态链都是对的，就可以这么找到了，而递推计算，当level为0时，静态链就是自身，level为1时，会去找0……
def get_sl(B, level):
    res_B = B
    while level > 0:
        res_B = stack[res_B]
        level -= 1
    return res_B


class Interpreter:
    def __init__(self):
        self.B = 0  # 基址寄存器
        self.T = 0  # 栈顶寄存器
        self.P = 0  # 下地址寄存器
        # 开始执行
        self.I = code[self.P]
        self.P += 1
        global window

    def end(self):
        return self.P == 0

    #  根据当前B的值和level层差获取SL的值
    # 因为生成代码的时候就是给定层差，而每个层差的上一层就是定义它的，所以是可行的

    def step(self):
        if not self.end():  # 因为退出程序有一个 OPR 0 0 的，所以看到self.P要到0就说明退出了
            if self.I['F'] == 'JMP':  # 直接跳转到对应指令
                self.P = self.I['A']
            elif self.I['F'] == 'JPC':
                if stack[self.T] == 0:  # 栈顶值为0才跳转
                    self.P = self.I['A']
                self.T -= 1  # 无论是否跳转都要去除栈顶的值
            elif self.I['F'] == 'INT':
                self.T += self.I['A'] - 1  # 开辟空间
            elif self.I['F'] == 'LOD':
                self.T += 1
                stack[self.T] = stack[get_sl(self.B, self.I['L']) + self.I['A']]  # 到了那层 找到真正基地址 再加addr
            elif self.I['F'] == 'STO':
                if self.I['L'] == -1:
                    stack[self.T + self.I['A']] = stack[self.T]
                    self.T -= 1
                else:
                    # print(get_sl(self.B, self.I['L']))
                    stack[get_sl(self.B, self.I['L']) + self.I['A']] = stack[self.T]
                    self.T -= 1
            elif self.I['F'] == 'LIT':
                self.T += 1
                stack[self.T] = self.I['A']
            elif self.I['F'] == 'CAL':  # 函数调用
                self.T += 1
                stack[self.T] = get_sl(self.B, self.I['L'])  # 静态链
                stack[self.T + 1] = self.B  # 动态链
                stack[self.T + 2] = self.P  # 返回地址-当前的下一条地址
                self.B = self.T  # 新的基地址
                self.P = self.I['A']  # 注意pro生成时填写的都是绝对地址code.len
                newbase.append(self.B)
            elif self.I['F'] == 'OPR':
                if self.I['A'] == 0:  # 函数返回
                    self.T = self.B - 1
                    self.P = stack[self.T + 3]
                    self.B = stack[self.T + 2]
                    newbase.pop()
                elif self.I['A'] == 1:  # 取反操作
                    stack[self.T] = -stack[self.T]
                elif self.I['A'] == 2:  # 加法
                    self.T -= 1
                    stack[self.T] = stack[self.T] + stack[self.T + 1]
                elif self.I['A'] == 3:  # 减法
                    self.T -= 1
                    stack[self.T] = stack[self.T] - stack[self.T + 1]
                elif self.I['A'] == 4:  # 乘法
                    self.T -= 1
                    stack[self.T] = stack[self.T] * stack[self.T + 1]
                elif self.I['A'] == 5:  # 除法
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] // stack[self.T + 1])
                elif self.I['A'] == 6:  # odd 奇偶
                    stack[self.T] = int(stack[self.T] % 2)
                elif self.I['A'] == 7:  # ==
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] == stack[self.T + 1])
                elif self.I['A'] == 8:  # <>
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] != stack[self.T + 1])
                elif self.I['A'] == 9:  # <
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] < stack[self.T + 1])
                elif self.I['A'] == 10:  # >
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] > stack[self.T + 1])
                elif self.I['A'] == 11:  # >=
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] >= stack[self.T + 1])
                elif self.I['A'] == 12:  # <=
                    self.T -= 1
                    stack[self.T] = int(stack[self.T] <= stack[self.T + 1])
            elif self.I['F'] == 'RED':
                self.T += 1
                # stack[self.T] = int(input("输入："))
                window.display_TEXT("请输入数字！")
                # print("输入数字！")
            elif self.I['F'] == 'WRT':
                # print("栈顶输出：", stack[self.T])
                window.output_STACK(str(stack[self.T]))
                self.T -= 1
            # print(self.P, stack[:T+8], T)
            self.I = code[self.P]  # 获取下一条指令
            # print(self.I)
            if self.P == 0:  # 因为退出程序有一个 OPR 0 0 的，所以看到self.P要到0就说明退出了
                return
            self.P += 1  # 默认self.P+1获取下一条指令 除非跳转


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("解释器演示")
        self.setFixedSize(500, 1000)

        self.scene = QGraphicsScene(self)
        self.view = QGraphicsView(self.scene, self)
        self.view.setGeometry(0, 0, 500, 1000)
        self.interpreter = Interpreter()

        self.start_button = QPushButton("开始", self)
        self.start_button.move(10, 10)
        self.start_button.clicked.connect(self.begin)

        self.next_button = QPushButton("下一条", self)
        self.next_button.move(120, 10)
        self.next_button.clicked.connect(self.handle_next)

        self.stop_button = QPushButton("暂停", self)
        self.stop_button.move(10, 150)
        self.stop_button.clicked.connect(self.handle_stop)

        self.label = QLabel("", self)
        self.label.move(280, 10)

        self.label2 = QLabel("栈顶输出：", self)
        self.label2.setFont(QFont("Arial", 10))
        self.label2.move(10, 100)
        self.label2.resize(400, 60)
        self.label2.setWordWrap(True)


        self.data_edit = QLineEdit(self)
        self.data_edit.move(10, 50)

        self.input_button = QPushButton("输入", self)
        self.input_button.move(120, 50)
        self.input_button.clicked.connect(self.handle_input)

        self.info = ""
        self.output = ""

        self.timeIntervals = 1
        self.send_time = QTimer(self)
        self.send_time.start(self.timeIntervals)
        # 给QTimer设定一个时间，每到达这个时间一次就会调用一次该方法
        self.send_time.timeout.connect(self.handle_next)

    def display(self):
        pen = QPen(QColor(0, 255, 0))
        font = QFont("Arial", 10)
        y = 2000
        for i in range(self.interpreter.T + 1):
            self.scene.addRect(50, y, 40, 25, pen)
            # 绘制栈中内容
            text = self.scene.addText(str(stack[i]), font)
            text.setDefaultTextColor(QColor(0, 0, 0))
            text.setPos(53, y)
            # 绘制标号
            index = self.scene.addText(str(i), font)
            index.setDefaultTextColor(QColor(0, 0, 0))
            if i < 10:
                index.setPos(25, y)
            else:
                index.setPos(22, y)
            if i in newbase:
                sl = self.scene.addText("静态链：", font)
                sl.setDefaultTextColor(QColor(0, 0, 0))
                sl.setPos(-50, y)
                dl = self.scene.addText("动态链：", font)
                dl.setDefaultTextColor(QColor(0, 0, 0))
                dl.setPos(-50, y-40)
                ra = self.scene.addText("返回地址：", font)
                ra.setDefaultTextColor(QColor(0, 0, 0))
                ra.setPos(-63, y-80)
            y -= 40
        # 绘制指令信息
        # 这里-1是在最后会自动加一,
        if not self.interpreter.end():
            order = "将要执行  " + str(self.interpreter.P - 1) + ": " + self.interpreter.I['F'] + " " + str(
                self.interpreter.I['L']) + " " + str(self.interpreter.I['A'])
            text2 = self.scene.addText(order, font)
            text2.setDefaultTextColor(QColor(0, 0, 0))
            text2.setPos(100, 2000)
        else:
            order = "程序运行结束"
            text2 = self.scene.addText(order, font)
            text2.setDefaultTextColor(QColor(0, 0, 0))
            text2.setPos(100, 2000)
        # 绘制提示
        if self.info != "":
            self.label.setText(self.info)
            self.info = ""
        else:
            self.label.setText("")
        # 绘制栈顶输出
        if self.output != "":
            self.label2.setText("栈顶输出：" + self.output)

        else:
            self.label2.setText("栈顶输出：")

    def display_TEXT(self, s):
        # 必须先输入才可以继续下一步
        self.next_button.setEnabled(False)
        self.info = s

    def output_STACK(self, s):
        self.output = self.output + s + " "
        # print(self.output)

    def begin(self):
        global newbase
        newbase.clear()
        newbase.append(0)
        self.output = ""
        self.interpreter.B = 0  # 基址寄存器
        self.interpreter.T = 0  # 栈顶寄存器
        self.interpreter.P = 0  # 下地址寄存器
        # 开始执行
        self.interpreter.I = code[self.interpreter.P]
        self.interpreter.P += 1
        self.interpreter.step()
        self.scene.clear()
        self.display()
        self.send_time.start(self.timeIntervals)
    def handle_next(self):
        # print(self.interpreter.T)
        if not self.next_button.isEnabled():
            return
        if not self.interpreter.end():
            self.data_edit.clear()
            self.interpreter.step()
            self.scene.clear()
            self.display()
        else:
            self.send_time.stop()
            self.start_button.setEnabled(True)

    def handle_input(self):
        data = self.data_edit.text()
        if data != "":
            stack[self.interpreter.T] = int(data)
            self.scene.clear()
            self.display()
            self.data_edit.clear()
            self.next_button.setEnabled(True)
    def handle_stop(self):
        if self.send_time.isActive():
            self.send_time.stop()
        else:
            self.send_time.start()



if __name__ == '__main__':
    origin = []
    with open("code.txt", 'r') as f:
        origin = f.readlines()
    for i in range(len(origin)):
        origin[i] = origin[i][:-1]
        x = origin[i].split()
        add_code(x[0], int(x[1]), int(x[2]))

    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
