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
    print('')

    
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
    cent_indexes = ""
    np.random.seed(0)
    new_data = pd.DataFrame(orginial_data)
    points = new_data.shape[0]
    center_index = np.random.choice(points)
    cent_indexes+=str(center_index)+","
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
        cent_indexes+=str(new_cent_index)+","
        new_cent_list = list(new_data.iloc[new_cent_index].values)
        cent_arr.append(new_cent_list)
    print(cent_indexes[:-1])
    return cent_arr
    

def join_data(input1, input2):
    df1 = pd.read_csv(input1, header=None)
    df2 = pd.read_csv(input2, header=None)
    data = pd.merge(df1, df2, on=0)
    sorted_data = data.sort_values(by=0, ascending=True)
    df_without_first_col = sorted_data.iloc[:, 1:]
    return df_without_first_col



    
if __name__== '__main__':
    cluster_error = False
    iter_error = False
    args = sys.argv

    # all provided 
    if len(args) == 6:
        K = int(args[1])
        num_iter = int ((args)[2])
        eps = float(args[3])
        input_data_1 = args[4]
        input_data_2 = args[5]
    
    # iter not provided
    elif len(args)==5:
        K = int(args[1])
        num_iter = 300
        try:
            eps = float(args[2])
        except ValueError:
            print("An Error Has Occurred")
            sys.exit(1)
        input_data_1 = args[3]
        input_data_2 = args[4]
        
        
    else:
        print("An Error Has Occurred")
        sys.exit(1)
    
    if not 1<num_iter<1000:
       print("Invalid maximum iteration!")
       iter_error = True
       
    data = join_data(input_data_1, input_data_2)
    N = data.shape[0]
    if not 1<K<N:
        print("Invalid number of clusters!")
        cluster_error= True
    
    
    if (cluster_error or iter_error):
        sys.exit(1)

    points_arr = data.values
    points = points_arr.tolist()

    centroids = kmeans_pp(K, data)
    d = len(centroids[0])
    try:
        final_c_centroids = mykmeanssp.fit(K, num_iter, d, eps, centroids,points)
    except RuntimeError as e:
        print("An Error Has Occurred")
        sys.exit(1)
    
    print_cent(final_c_centroids)
    sys.exit(0)
