import sys
import numpy as np
import pandas as pd
import math
import mykmeanssp

def print_cent(centroids):
    for lst in centroids:
        res = ""
        for i in range(len(lst)):
            if i!= len(lst)-1:
                res += '%.4f'%(lst[i])+","
            else:
                res += '%.4f'%(lst[i])
        print(res)
    print('\n')
    
def cal_dist(x1, x2):
    res = 0.0
    for i in range(len(x1)):
        res += ((x1[i])-(x2[i]))**2
    return math.sqrt(res)


def comp_d(center_arr, point):
    min_dist = math.inf
    cur_dist =0.0
    for cent in center_arr:
        cur_dist = cal_dist(cent, point)
        min_dist = min(cur_dist, min_dist)
    return min_dist
    

def kmeans_pp(K, orginial_data):
    cent_arr=[]
    cent_indexes = []
    np.random.seed(0)
    new_data = pd.DataFrame(orginial_data)
    points = new_data.shape[0]
    center_index = np.random.choice(points)
    cent_indexes.append(center_index)
    center = list(new_data.iloc[center_index].values)
    cent_arr.append(center)
    distances = [-1 for i in range(points)]

    for j in range(K-1):
        for i in range(new_data.shape[0]):
            point = new_data.iloc[i].values
            d = comp_d(cent_arr, point) #min dist between point and centroids
            distances[i]= d
            
            
        sum_dist = sum(distances)
        prob = [distances[i]/sum_dist for i in range(points)] 
        new_cent_index = np.random.choice(points, p=prob)
        cent_indexes.append(new_cent_index)
        new_cent_list = list(new_data.iloc[new_cent_index].values)
        cent_arr.append(new_cent_list)
    print(cent_indexes)
    return cent_arr
    

def join_data(input1, input2):
    df1 = pd.read_csv(input1, header=None)
    df2 = pd.read_csv(input2, header=None)
    data = pd.merge(df1, df2, on=0)
    sorted_data = data.sort_values(by=0, ascending=True)
    df_without_first_col = sorted_data.iloc[:, 1:]
    return df_without_first_col



    
# if __name__== '__main__':
#     args = sys.argv
#     # all provided 
#     if len(args) == 6:
#         K = int(args[1])
#         num_iter = int ((args)[2])
#         eps = args[3]
#         input_data_1 = args[4]
#         input_data_2 = args[5]
    
#     # iter not provided
#     elif len(args)==5:
#         K = int(args[1])
#         num_iter = 300
#         eps = args[3]
#         input_data_1 = args[4]
#         input_data_2 = args[5]
        
#     else:
#         raise ValueError("An Error Has Occurred")
def main(K, iter, eps, input_data_1, input_data_2):
    num_iter = iter
    data = join_data(input_data_1, input_data_2)
    N = data.shape[0]
    if not 1<K<N:
        raise ValueError("Invalid number of clusters!")
    if not 1<iter<1000:
        raise ValueError("Invalid maximum iteration!")
    

    points_arr = data.values
    points = points_arr.tolist()
    string_of_points = ""
    for i in range(N):
        vec = ','.join(points[i]) +'\n'
        string_of_points += vec
        
    centroids = kmeans_pp(K, data)
    d = len(centroids[0])


    final_c_centroids = mykmeanssp.fit(K, num_iter, d, eps, centroids,points)
    print_cent(final_c_centroids)
        


main(3,  333, 0, "input_1_db_1.txt", "input_1_db_2.txt" )