import numpy as np
import sys
import subprocess
from sklearn.metrics import silhouette_score
import symnmf
import math
from kmeans import kmeans




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
        
def running_kmeans(k, file_path):
    temp_cent = kmeans(k, file_path)
    return temp_cent
    
def running_symnmf_py(k,file_path):
    data = read_data(file_path)
    n = len(data)
    dim = len(data[0])
    symnmf_mat = symnmf_env(k, n, dim, data)
    return symnmf_mat


def max_arg(lst):
    lst = [float(val) for val in lst]
    max_val = max(lst)
    return lst.index(max_val)

def kmeans_get_labels(temp_cent, data_points):    
    labels = []
    f = open(data_points, "r")
    lines = f.readlines()
    for line in lines:
        line = line.strip()
        point = line.split(",")
        for i in range(len(temp_cent)):
            if point in temp_cent[i]:
                labels.append(i)
                continue
    return labels
        
    
      
def read_data(file_path):
    with open(file_path, "r") as file:
        lines = file.readlines()
    data_list = [[float(val) for val in line.strip().split(",")] for line in lines]
    return data_list
    
    
        
def symnmf_get_labels(symnmf_mat):
    labels =[]
    for row in symnmf_mat:
        max_index = max_arg(row)
        labels.append(max_index)
    return labels

def silhouette_calculation(data, symnmf_labels, kmeans_labels):
    silhouette_symnmf = silhouette_score(data, symnmf_labels)
    silhouette_kmeans = silhouette_score(data, kmeans_labels)
    print("nmf:", '%.4f'%silhouette_symnmf)
    print("kmeans:", '%.4f'%silhouette_kmeans)




if __name__ == "__main__":
    args = sys.argv
    k = int(args[1])
    file_path = args[2]
    symnmf_mat = running_symnmf_py(k,file_path)
    if symnmf_mat == -1:
        exit()
        
    temp_cent = running_kmeans(k,file_path)
    symnmf_labels = symnmf_get_labels(symnmf_mat)
    kmeans_labels= kmeans_get_labels(temp_cent, file_path)
    data_points = read_data(file_path)
    silhouette_calculation(data_points, symnmf_labels, kmeans_labels)
    
    