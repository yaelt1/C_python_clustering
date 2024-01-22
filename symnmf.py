import math
import symnmf
import sys
import numpy as np


def print_mat(mat):
    for row in mat:
        res = ""
        for i in range(len(row)):
            if i!= len(row)-1:
                res += '%.4f'%(row[i])+","
            else:
                res += '%.4f'%(row[i])
        print(res)
    

def cal_dist(x1 :list, x2:list):
    res = 0.0
    for i in range(len(x1)):
        res += (float(x1[i])-float(x2[i]))**2
    return res

def str_to_float(data):
    for i in range(len(data)):
        for j in range(len(data[0])):
            data[i][j] = float(data[i][j])
    return data

def read_file(path):
    try:
        with open(path, "r") as f:
            data = f.read().split("\n")
    except:
        return None
    data = [data[i].split(",") for i in range(len(data))]
    while [''] in data:
        data.remove([''])
    data = str_to_float(data)
    return data

def init_H(k, n, dim, points):
    m = symnmf.fit(0, n ,dim,k, points) #m = average of all entries of W
    limit = 2*math.sqrt(m/k)
    np.random.seed(0)
    H0 = [[0 for i in range(k)] for j in range(n)]
    for i in range(n):
        for j in range(k):
            H0[i][j] = np.random.uniform(0, limit)
    return H0

def symnmf_env(k, n, dim, points):
        H_0 = init_H(k, n, dim, points) 
        try:
            w_mat = symnmf.fit(3, n ,dim,k, points)
        except RuntimeError as e:
            print("An Error Has Occurred")
            return -1
        try:
            symnmf_mat = symnmf.fit(4, n ,dim,k, points, w_mat,H_0)
            return symnmf_mat
        
        except RuntimeError as e:
            print("An Error Has Occurred")
            return -1    
    
def call_func(goal,n, points, k):
    if goal=="sym":
        try:
            sim_mat = symnmf.fit(1, n ,dim,k, points)
            print_mat(sim_mat)
        except RuntimeError as e:
            print("An Error Has Occurred")
            sys.exit(1)
       
         
    elif goal == "ddg":
        try:
            ddg_mat = symnmf.fit(2, n ,dim,k, points)
            print_mat(ddg_mat)
        except RuntimeError as e:
            print("An Error Has Occurred")
            sys.exit(1)
        
    elif goal == "norm":
        try:
            norm_mat = symnmf.fit(3, n ,dim, k,points)
            print_mat(norm_mat)
        except RuntimeError as e:
            print("An Error Has Occurred")
            sys.exit(1)
        
    elif goal == "symnmf":
        symnmf_mat = symnmf_env(k, n, dim, points)
        if symnmf_mat == -1:
            sys.exit(1)
            
        print_mat(symnmf_mat)
    


if __name__ =="__main__":
    args = sys.argv
    if len(args) == 4:
        args = sys.argv
        k = int(args[1])
        goal = args[2]
        path_to_input = args[3]
    else:
        print("An Error Has Occurred")
        sys.exit(1)
    
    data = read_file(path_to_input)
    if data == None:
        print("An Error Has Occurred")
        sys.exit(1)
        
    n = len(data) #number of points in data
    dim = len(data[0]) #dims of points   
    call_func(goal,n ,data, k)
    

