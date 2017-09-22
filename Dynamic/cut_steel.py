"""
 链接：http://blog.csdn.net/gqtcgq/article/details/45530443

 给定一个长度为n的钢条，以及一个价格表p，
 p中列出了每英寸钢条的价格，将长度为n的钢条切割为若干短钢条出售，
 求一个钢条的切割方案，使得收益最大
"""

def CUT_ROD(p, n):
    if n == 0:
        return 0
    q = -1
    # 假设前i长度的钢条不切割，
    # 该切割方案的收益为：前i长度的价格 + CUT-ROD函数处理剩下的钢条返回的最优收益
    # 如果此时总长度为n，则有n种方案要进行比较大小
    # i = n 时表示不分割
    for i in range(1, n + 1):
        q = max(q, p[i] + CUT_ROD(p, n - i))
        # print(i, q)
    return q

def MEMORY_CUT_ROD(p, n):

    def MEMORY_CUT_ROD_AUX(p, n, b):
        if b[n] >= 0:
            return b[n]
        # n为0时,b[0] = 0
        if n == 0:
            q = 0
        else:
            q = -1
        for i in range(1, n + 1):
            q = max(q, p[i] + MEMORY_CUT_ROD_AUX(p, n - i, b))
        b[n] = q
        return q

    """
    自顶向下递归的时候，记录方案的结果，防止重复计算

    需要一个数组来记录0..n个长度的钢条的最大收益
    """
    book = [-1] * (n + 1)
    return MEMORY_CUT_ROD_AUX(p, n, book)

def BOTTOM_UP_CUT_ROD(p, n):
    """
    自底向上求解，从小规模的问题开始计算
    """
    book = [-1] * (n + 1)
    book[0] = 0
    # i 为钢条长度
    for i in range(1, n + 1):
        q = -1
        # j为切割点
        for j in range(1, i + 1):
            # i -j 为切割后剩下的长度
            q = max(q, p[j] + book[i - j])
        book[i] = q
    return book[n]
if __name__ == '__main__':
    parry = [0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30]

    # print(CUT_ROD(parry, 4))
    # print(MEMORY_CUT_ROD(parry, 4))
    print(BOTTOM_UP_CUT_ROD(parry, 4))