import numpy as np
import pandas as pd
import sys
import kmeans
def kMeans_pp(dataPoints, k, epsilon, iter=300 ):
    
    
    dataPoints_array = np.array(dataPoints)
    centroids = []
    n = len(dataPoints_array)
    
    distances = np.zeros(n)
    centroids.append(dataPoints_array[np.random.choice(n)].copy())
    
    for _ in range(k-1):
        index = 0
        for point in dataPoints_array:
            distances[index] = findClosestCenter(point,centroids)
            index += 1
            
        probabilities  = distances / distances.sum()
        next_centroid = np.random.choice(n,p = probabilities)
        centroids.append(dataPoints_array[next_centroid].copy())
    
    centroids_array = np.array(centroids)
    centroids_array = centroids_array.tolist()
    dataPoints_array = dataPoints_array.tolist()
    c = kmeans.fit(centroids_array, dataPoints_array, iter, epsilon)
    print(c)
        

def findClosestCenter(datapoint, centroids):
    minDistance = calcEclideanDistance(np.array(centroids[0]), datapoint[1])
    for u in enumerate(centroids):
        distance = calcEclideanDistance(u[1], datapoint[1])
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
        epsilon = float(args[2])
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
        
    df1 = pd.read_csv(filePath1, header=None)
    df2 = pd.read_csv(filePath2, header=None)
    df1.set_index(0, inplace=True)
    df2.set_index(0, inplace=True)
    final_filepath = df1.join(df2, how='inner', lsuffix='_file1', rsuffix='_file2')
    final_filepath.reset_index(inplace=True)
    final_filepath = final_filepath.sort_values(by=final_filepath.columns[0])
    final_filepath = final_filepath.drop(final_filepath.columns[0], axis=1)
    final_filepath = final_filepath.reset_index(drop=True)
    print(final_filepath)
    list = final_filepath.to_numpy()
    index_list =final_filepath.index.tolist()
    print(index_list)
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
   
    return k, iter,list,epsilon, filePath1,filePath2

if __name__ == '__main__':
    k,iter,list,epsilon, filePath1,filePath2, = parseArgs(sys.argv)
    try:
        kMeans_pp(list, k,epsilon, iter)
    except:
        print("An Error Has Occurred")
        exit()
        