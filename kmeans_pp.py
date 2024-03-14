import numpy as np
import pandas as pd
import sys
import kmeans

def kMeans_pp(dataPoints, k, epsilon, iter=300 ):
    centroids = []
    
    n = len(dataPoints)
    distances = np.zeros(n)
    np.random.seed(0)
    centroids.append(dataPoints[np.random.choice(n)].copy())
    
    for _ in range(k-1):
        index = 0
        for point in dataPoints:
            distances[index] = findClosestCenter(point,centroids)
            index += 1
            
        probabilities  = distances / distances.sum()
        next_centroid = np.random.choice(n,p = probabilities)
        centroids.append(dataPoints[next_centroid].copy())
    
    centroids_array = np.array(centroids)
    centroids_array = centroids_array.tolist() 
    
    return centroids_array    

def findClosestCenter(datapoint, centroids):
    minDistance = calcEclideanDistance(centroids, datapoint)
    for u in centroids:
        distance = calcEclideanDistance(u, datapoint)
        if(distance < minDistance):
            minDistance = distance
    return minDistance

def calcEclideanDistance(u, v):
    return np.sqrt(np.sum((u - v) ** 2))

def parseArgs(args):
    if len(args) == 5:
        iter = 300
        filePath1 = args[3]
        filePath2 = args[4]
        try:
            epsilon = float(args[2])
            if not(epsilon >= 0):
                exit()
        except: 
            print("Invalid epsilon!")
            exit()
        
    if len(args) == 6:
        try:
            iter = int(args[2])
            if(iter < 2 or iter >= 1000):
                exit()
        except:
            print("Invalid maximum iteration!")
            exit()
        filePath1 = args[4]
        filePath2 = args[5]
        epsilon = float(args[3])
        try:
            epsilon = float(args[2])
            if not(epsilon >= 0):
                exit()
        except: 
            print("Invalid epsilon!")
            exit()
    try:
        k = int(args[1]) 
    except:
        print("Invalid number of clusters!")    
        exit()
   
    return k, iter, epsilon, filePath1, filePath2

def parseFiles(filePath1, filePath2, k):
    df1 = pd.read_csv(filePath1, header=None)
    df2 = pd.read_csv(filePath2, header=None)
    final_filepath = pd.merge(df1,df2, how='inner', on=0)
    index_list = final_filepath[0].tolist()
    index_list.sort()
    final_filepath = final_filepath.sort_values(by = 0) 
    final_filepath = final_filepath.loc[: , '1_x':]
    list = final_filepath.to_numpy()
     
    if(len(list)==0):
        print("An Error Has Occurred")
        exit()    
        
    if(k < 2 or k >= len(list)):
        print("Invalid number of clusters!")
        exit()   
    
    return list, index_list

if __name__ == '__main__':
    k, iter, epsilon, filePath1, filePath2 = parseArgs(sys.argv)
    list, index_list = parseFiles(filePath1, filePath2, k)
    
    try:
        init_centroids = kMeans_pp(list, k, epsilon, iter)
        dataPoints_array = list.tolist()     
        centroidIndices = []
        
        for centroid in init_centroids: 
            for index, dataPoint in enumerate(list):
                if(np.array_equal(centroid,dataPoint)):
                    centroidIndices.append(index)
        
        c = kmeans.fit(init_centroids, dataPoints_array, iter, epsilon)
        
        print(','.join(map(str,centroidIndices)))
            
        for u in c:
            formatted = [ '%.4f' % elem for elem in u ]
            print(','.join(formatted))
        
    except:
        print("An Error Has Occurred")
        exit()
        