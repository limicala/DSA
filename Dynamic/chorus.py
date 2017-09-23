# -*- coding: utf-8 -*-
"""
  题目链接：https://www.nowcoder.com/practice/661c49118ca241909add3a11c96408c8

  题目描述
  有 n 个学生站成一排，每个学生有一个能力值，
  牛牛想从这 n 个学生中按照顺序选取 k 名学生，
  要求相邻两个学生的位置编号的差不超过 d，
  使得这 k 个学生的能力值的乘积最大，你能返回最大的乘积吗？
------------------------------------------------------------------------------

  若先考虑不限制相邻学生的位置编号之差
  则问题可以先转换为 在n个数字中选出k个数，使得乘积最大，
  这个问题就是求最大乘积子序列

  可以先假设最后结果的k个数中的最后一个数的位置last
  
  r(last, k) 表示从n中选出k个数的最优方案，其中最后一个数是第last个
  r(last, k) :
    for i in (k - 1, last)：
        # 能力值可以为正负，所以要计算最小值和最大值
        max(r(i, k - 1) * student[last])

        min(r(i, k - 1) * student[last])

  如果要考虑限制相邻学生的位置编号之差d时，
  其实也就是限制k个数的最后一个数的位置范围

  r(last, k) :
    for i in (max(k - 1, last - d) , last)：
        # 能力值可以为正负，所以要计算最小值和最大值
        max(r(i, k - 1) * student[last])

        min(r(i, k - 1) * student[last])
"""
def solve1(s, n, k):
    """
    从n个数字中选出k个数，返回最大乘积
    :param s:
    :param n:
    :param k:
    :return: 最大乘积
    """
    if k == 1:
        return min(s[0:n]), max(s[0:n])

    q = -1
    curr_min = 999999999
    curr_max = -1
    for i in range(k - 1, n):
        # 枚举选定最后第k个数的位置

        """
        eg:
        n = 5, k = 3
        ________________
        i    |0 1 2 3 4
        s[i] |1 4 3 2 5
        
        当i = 2选为第k个数时，那就剩下2个数用来选出剩余的k - 1个数
        
        所以下一个递归方法的参数 n := i, k := k - 1  
        """
        pre_min, pre_max = solve1(s, i, k - 1)

        curr_min = min(curr_min, pre_min * s[i], pre_max * s[i])
        curr_max = max(curr_max, pre_min * s[i], pre_max * s[i])

        # if s[i] > 0:
        #     q = max(q, curr_max * s[i])
        # else:
        #     q = max(q, curr_min * s[i])

    return curr_min, curr_max
# 递归
# 在牛客网上提交结果：
#
# 运行超时:您的程序未能在规定时间内运行结束，请检查是否循环有错或算法复杂度过大。
# case通过率为80.00%
def solve2(s, n, k, d, book=None):
    """
    加了限制条件相邻两个数之差d的条件
    其实与求解最大乘积子序列问题类似
    只是限制了第k个数的位置范围

    有个隐藏条件, n表示可以有n个数可以选出k个,也表明第n+1个数就是被选出来的数
    :param s:
    :param n:
    :param k:
    :param d:
    :return:
    """
    print("n", n, "k", k)
    if k == 0:
        return 1, 1



    # 选定i的最小值
    # 第一次执行此方法时, 第k个数还未选好,所以取值不受d限制
    left = k - 1

    if len(s) != n:
        """
        eg:
        n = 8, k = 3, d = 3
        ______________________
        i    |0 1 2 3 4 5 6 7
        s[i] |1 4 3 2 5 9 5 2

        当i = 6选为第k个数时，那就剩下6个数用来选出剩余的k - 1个数
        下一次的方法中 n = 6
        在没有d的限制之前，第k-1的选择范围为[k - 1, n),也就是[2,6)
        由于d的限制，所以第k-1个数的选定范围是
        [max(n - d, k - 1) ,6) => [max(3, 2),6) => [3,6)
        """
        left = max(k - 1, n - d)
        # print ("k - 1", k - 1, "left", left)
    # if k == 1:
    #     return max(s[left:n])


    q = -1
    curr_min = 999999999
    curr_max = -1
        # print ("aaa")
    print ("left" , left, "n", n)
    for i in range(left, n):
        # 枚举选定最后第k个数的位置

        """
        eg:
        n = 8, k = 3, d = 4
        ______________________
        i    |0 1 2 3 4 5 6 7
        s[i] |1 4 3 2 5 9 5 2

        当i = 6选为第k个数时，那就剩下6个数用来选出剩余的k - 1个数
        
        所以下一个递归方法的参数 n := i, k := k - 1  
        """
        # print("n", i, "k", k - 1)
        # pre = solve2(s, i, k - 1, d)
        #
        # curr_min = min(curr_min, result)
        # curr_max = max(curr_max, result)
        pre_min, pre_max = solve2(s, i, k - 1, d)

        curr_min = min(curr_min, pre_min * s[i], pre_max * s[i])
        curr_max = max(curr_max, pre_min * s[i], pre_max * s[i])
        # print ("i", i , "curr_min" , curr_min, "curr_max", curr_max)
        # if student[i] > 0:
        #     q = max(q, curr_max * s[i])
        # else:
        #     q = max(q, curr_min * s[i])
    return curr_min, curr_max

# 备忘录

# 在牛客网上提交结果：
#
# 运行超时:您的程序未能在规定时间内运行结束，请检查是否循环有错或算法复杂度过大。
# case通过率为90.00%
def book_recursion(s, n, k, d):

    def solve(s, n, k, d, m1, m2):
        if min_matrix[n][k] > 0:
            return min_matrix[n][k], max_matrix[n][k]
        left = k - 1
        if len(s) != n:
            left = max(k - 1, n - d)
        q = -1
        curr_min = 999999999
        curr_max = -1
        for i in range(left, n):
            pre_min, pre_max = solve(s, i, k - 1, d, m1, m2)
            curr_min = min(curr_min, pre_min * s[i], pre_max * s[i])
            curr_max = max(curr_max, pre_min * s[i], pre_max * s[i])
        min_matrix[n][k] = curr_min
        max_matrix[n][k] = curr_max
        return curr_min, curr_max

    min_matrix = [[0 for i in range(k + 1)] for i in range(n + 1)]
    max_matrix = [[0 for i in range(k + 1)] for i in range(n + 1)]

    for i in range(n + 1):
        min_matrix[i][0] = 1
        max_matrix[i][0] = 1

    return solve(s, n, k, d, min_matrix, max_matrix)

def dynamic(s, n, k, d):

    min_matrix = [[1 for i in range(k + 1)] for i in range(n + 1)]
    max_matrix = [[1 for i in range(k + 1)] for i in range(n + 1)]

    result = 0
    for kk in range(1, k + 1):

        for i in range(1,n + 1):
            """
            由于d的条件限制，
            eg:
            n = 8, k = 3, d = 4
            ______________________
            i    |0 1 2 3 4 5 6 7
            s[i] |1 4 3 2 5 9 5 2
            
            当i = 6选为第k个数时，那就剩下6个数用来选出剩余的k - 1个数
            由于d = 4
            所以min[6][3] = min( min[5][2], ... min[2][2])
             
            """
            for dd in range(1, d + 1):
                if i - dd >= 0:
                    a = min_matrix[i - dd][kk - 1] * s[i - 1]
                    b = max_matrix[i - dd][kk - 1]
                    min_matrix[i][kk] = min(min_matrix[i][kk], a, b)
                    max_matrix[i][kk] = max(max_matrix[i][kk], a, b)

            result = max(result, max_matrix[i][kk])
        # print (max_matrix[4][kk])
    return result

if __name__ == '__main__':
    student = [-7, 45, -1, -2, 8, 1]
    k = 3
    d = 20
    # print (solve1(student, len(student), k))
    # print (solve1_max(student, len(student), k))
    # print (solve2_max(student, len(student), k, d))
    # print (solve2(student, len(student), k, d))


    # print (book_recursion(student, len(student), k, d))
    print (dynamic(student, len(student), k, d))

    # min_matrix = [[1 for i in range(k + 1)] for i in range(5 + 1)]

    # print (min_matrix[1,2,3][1])
