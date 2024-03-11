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
        
    df1 = pd.read_csv(filePath1, header=None)
    df2 = pd.read_csv(filePath2, header=None)
    df1.set_index(0, inplace=True)
    df2.set_index(0, inplace=True)
    final_filepath = df1.join(df2, how='inner', lsuffix='_file1', rsuffix='_file2')

    list = final_filepath.to_numpy()
    index_list = final_filepath.index.tolist()
    index_list.sort()
    final_filepath.reset_index(inplace=True)
    final_filepath = final_filepath.sort_values(by=final_filepath.columns[0])
    final_filepath = final_filepath.drop(final_filepath.columns[0], axis=1)
    final_filepath = final_filepath.reset_index(drop=True)
    list = final_filepath.to_numpy()
    
    if(len(list)==0):
        print("An Error Has Occurred")
        exit()    
    
    try:
        k = int(args[1])
        if(k < 2 or k > len(list)):
            exit()    
    except:
        print("Invalid number of clusters!")    
        exit()
   
    return k, iter, list, index_list, epsilon, filePath1, filePath2

if __name__ == '__main__':
    k, iter, list, index_list, epsilon, filePath1, filePath2 = parseArgs(sys.argv)
    try:
        init_centroids = kMeans_pp(list, k, epsilon, iter)
        dataPoints_array = list.tolist()     
        centroidIndices = []
        
        for centroid in init_centroids: 
            for index, dataPoint in enumerate(list):
                if(np.array_equal(centroid,dataPoint)):
                    centroidIndices.append(index)
                    
       
        
        c= kmeans.fit(init_centroids, dataPoints_array, iter, epsilon)
        
        print(','.join(map(str,centroidIndices)))
            
        for u in c:
            formatted = [ '%.4f' % elem for elem in u ]
            print(','.join(formatted))
        
    except:
        print("An Error Has Occurred")
        exit()
        