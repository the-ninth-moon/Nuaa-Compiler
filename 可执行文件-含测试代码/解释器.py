stack = [0 for i in range(0, 8000)]  # 数据栈 前三个0是主函数的SL DL RA
code = []  # 存放代码


#  根据当前B的值和level层差获取SL的值
# 因为生成代码的时候就是给定层差，而每个层差的上一层就是定义它的，所以是可行的
def get_sl(B, level):
    res_B = B
    # 由当前的basic去找最近的直接外层，B是沿着静态链找的，因为要找最新的
    # 只要保证每一个静态链都是对的，就可以这么找到了，而递推计算，当level为0时，静态链就是自身，level为1时，会去找0……
    while level > 0:
        res_B = stack[res_B]
        level -= 1
    return res_B


def add_code(f, l, a):
    operation = dict()
    operation['F'] = f
    operation['L'] = l
    operation['A'] = a
    code.append(operation)


def interpreter():
    B = 0  # 基址寄存器
    T = 0  # 栈顶寄存器
    I = None  # 存放要执行的代码
    P = 0  # 下地址寄存器
    # 开始执行
    I = code[P]
    P += 1
    while P != 0:  # 因为退出程序有一个 OPR 0 0 的，所以看到P要到0就说明退出了
        if I['F'] == 'JMP':  # 直接跳转到对应指令
            P = I['A']
        elif I['F'] == 'JPC':
            if stack[T] == 0:  # 栈顶值为0才跳转
                P = I['A']
            T -= 1  # 无论是否跳转都要去除栈顶的值
        elif I['F'] == 'INT':
            T += I['A'] - 1  # 开辟空间
        elif I['F'] == 'LOD':
            T += 1
            stack[T] = stack[get_sl(B, I['L']) + I['A']]  # 到了那层 找到真正基地址 再加addr
        elif I['F'] == 'STO':
            if(I['L']==-1):
                stack[T+I['A']] = stack[T]
                T -= 1
            else:
                stack[get_sl(B, I['L']) + I['A']] = stack[T]
                T -= 1
        elif I['F'] == 'LIT':
            T += 1
            stack[T] = I['A']
        elif I['F'] == 'CAL':  # 函数调用
            T += 1
            stack[T] = get_sl(B, I['L'])  # 静态链
            stack[T + 1] = B  # 动态链
            stack[T + 2] = P  # 返回地址-当前的下一条地址
            B = T  # 新的基地址
            P = I['A']  # 注意pro生成时填写的都是绝对地址code.len
        elif I['F'] == 'OPR':
            if I['A'] == 0:  # 函数返回
                T = B - 1
                P = stack[T + 3]
                B = stack[T + 2]
            elif I['A'] == 1:  # 取反操作
                stack[T] = -stack[T]
            elif I['A'] == 2:  # 加法
                T -= 1
                stack[T] = stack[T] + stack[T + 1]
            elif I['A'] == 3:  # 减法
                T -= 1
                stack[T] = stack[T] - stack[T + 1]
            elif I['A'] == 4:  # 乘法
                T -= 1
                stack[T] = stack[T] * stack[T + 1]
            elif I['A'] == 5:  # 除法
                T -= 1
                stack[T] = int(stack[T] // stack[T + 1])
            elif I['A'] == 6:  # odd 奇偶
                stack[T] = int(stack[T] % 2)
            elif I['A'] == 7:  # ==
                T -= 1
                stack[T] = int(stack[T] == stack[T + 1])
            elif I['A'] == 8:  # <>
                T -= 1
                stack[T] = int(stack[T] != stack[T + 1])
            elif I['A'] == 9:  # <
                T -= 1
                stack[T] = int(stack[T] < stack[T + 1])
            elif I['A'] == 10:  # >
                T -= 1
                stack[T] = int(stack[T] > stack[T + 1])
            elif I['A'] == 11:  # >=
                T -= 1
                stack[T] = int(stack[T] >= stack[T + 1])
            elif I['A'] == 12:  # <=
                T -= 1
                stack[T] = int(stack[T] <= stack[T + 1])
        elif I['F'] == 'RED':
            T += 1
            stack[T] = int(input("输入："))
        elif I['F'] == 'WRT':
            print("栈顶输出：", stack[T])
            T -= 1
        #print(P, stack[:T+8], T)
        I = code[P]  # 获取下一条指令
        #print(I)
        if P == 0:  # 因为退出程序有一个 OPR 0 0 的，所以看到P要到0就说明退出了
            break
        P += 1  # 默认P+1获取下一条指令 除非跳转


if __name__ == '__main__':
    origin = []
    with open("code.txt", 'r') as f:
        origin = f.readlines()
    for i in range(len(origin)):
        origin[i] = origin[i][:-1]
        x = origin[i].split()
        add_code(x[0], int(x[1]), int(x[2]))
    interpreter()
    input()