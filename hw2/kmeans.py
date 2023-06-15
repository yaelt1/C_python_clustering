import math
import sys

def cal_dist(x1, x2):
    res = 0.0
    for i in range(len(x1)):
        res += (float(x1[i])-float(x2[i]))**2
    return math.sqrt(res)

def cal_avg(lst):
    dim = len(lst[0])
    res = [0.0 for i in range(dim)]
    for point in lst:
        for i in range(dim):
            res[i]+=  float(point[i])
    for i in range(dim):
        res[i] = '%.4f'%(res[i]/len(lst))
    return res  
   
def print_cent(centroid):
    for lst in centroid:
        str = ""
        for i in range(len(lst)):
            if i!= len(lst)-1:
                str += lst[i]+","
            else:
                str += lst[i]
        print(str)
    print("")
            
def kmeans(K, file_name, iter=200):
    err1= False
    err2= False
    err3= False
    eps = 0.01
    if type(iter)!= int or  iter>=1000 or iter<=1:
        err1= True
        print("Invalid maximum iteration!")
    try:
        f = open(file_name, "r")
        data = f.read().split("\n")
    except:
        print("An Error Has Occurred")
        err2 = True
    
    data = [data[i].split(",") for i in range(len(data))]
    data.remove([''])
    N = len(data)
    
    
    if type(K)!=int or K<=1 or K>= N:
        print("Invalid number of clusters!")
        err3 = True
   
    if err1 or err2 or err3:
        raise ValueError()
        
    centroids = [data[i] for i in range(K)]
    cnt = 0
    max_dist = -1* math.inf
    to_break = False
    while cnt < iter and to_break==False:
        temp_centroids = [[] for i in range(K)]
        for point in data:
            dists = [cal_dist(point, cent) for cent in centroids]
            min_cent_index = dists.index(min(dists))
            temp_centroids[min_cent_index].append(point)   
        new_cent = [cal_avg(temp_centroids[i]) for i in range(K)]
        cnt +=1 
        for i in range(K):
            temp_dist = cal_dist(new_cent[i], centroids[i])
            if temp_dist> max_dist:
                max_dist = temp_dist
        if max_dist >eps:
            centroids = new_cent
        else: 
            to_break = True 
    
    
    print_cent(centroids)
    


if __name__== '__main__':
    args = sys.argv
    if len(args) == 4:
        K = int(args[1])
        num_iter = int ((args)[2])
        input_data = args[3]
    elif len(args)==3:
        K = int(args[1])
        num_iter = 200
        input_data = args[2]
    else:
        raise ValueError("An Error Has Occurred")
    kmeans(K, input_data, num_iter)
        
        
