#   Author: Rachel Law
#   ID 861071722
#   rlaw001@ucr.edu
#   12/10/15

#   Project 2 - Feature Search
#   CS 170: Introduction to Artificial Intelligence
#   Instructor: Dr. Eamonn Keogh
import sys
import math
print (sys.version)


# return index of nearest neighbor to index_itema
# based on the euclidean distances of evaluating the 
# features in feature_set
def GetNearestNeighbor(dataset, feature_set, index_itema):
    current_closest_neighbor = index_itema
    current_closest_distance = -1.0
    for itemb in range(len(dataset)):
        distance = 0.0
        for feature in feature_set:
            distance += math.pow((dataset[index_itema][feature] - dataset[itemb][feature]), 2)
        if distance < current_closest_distance or current_closest_distance < 0.0 :
            if index_itema != itemb :
                current_closest_neighbor = itemb
                current_closest_distance = distance
    return current_closest_neighbor

# returns the accuracy of a feature set via K-Cross-Validation technique
def KCrossValidation(dataset, feature_set, k=1):
    if len(feature_set)==0 : return 0.0
    correct = incorrect = 0.0
    for item in range(len(dataset)):
        nearest_neighbor = GetNearestNeighbor(dataset, feature_set, item)
        if dataset[nearest_neighbor][0]==dataset[item][0] : correct += 1
        else : incorrect += 1
    return correct/(correct+incorrect)

# Finds the ideal feature set starting from an empty set and adding features
def Forward_Search(dataset):
    best_total_feature_set = []
    best_total_feature_set_accuracy = 0.0

    best_level_feature_set = []
    best_level_feature = 0
    best_level_feature_set_accuracy = 0.0

    current_feature_set = []
    current_feature_set_accuracy = 0.0

    features_to_consider = [i for i in range(1,len(dataset[0]))]
    for tree_level in range(1,len(dataset[0])):
        print("On the ", tree_level ,"th level of the search tree")
        best_level_feature_set_accuracy = 0.0
        best_level_feature = 0
        current_feature_set = best_level_feature_set
        for feature in features_to_consider:
            current_feature_set.append(feature)
            current_feature_set_accuracy = KCrossValidation(dataset, current_feature_set)
            print("--Considering adding the ", feature, " feature with accuracy ", current_feature_set_accuracy)
            if current_feature_set_accuracy > best_level_feature_set_accuracy :
                best_level_feature_set_accuracy = current_feature_set_accuracy
                best_level_feature = feature
            current_feature_set.remove(feature)
        best_level_feature_set.append(best_level_feature)
        features_to_consider.remove(best_level_feature)
        print("----The best_level_feature_set is now ", best_level_feature_set, " with accuracy ", best_level_feature_set_accuracy)
        if best_level_feature_set_accuracy > best_total_feature_set_accuracy :
            best_total_feature_set_accuracy = best_level_feature_set_accuracy
            best_total_feature_set = best_level_feature_set[:]
    print("\nThe best set combination was ", best_total_feature_set, " with accuracy ", best_total_feature_set_accuracy)

# Finds the ideal feature set based on starting from a full set and eliminating features
def Backward_Search(dataset):
    best_feature_set = []
    best_feature_set_accuracy = -1.0

    cur_feature_set = []
    cur_feature_set_accuracy = -1.0

    level_feature_set = [i for i in range(1,len(dataset[0]))]
    level_feature_set_accuracy = -1.0

    features_to_remove = [i for i in range(1,len(dataset[0]))]

    for tree_level in range(1,len(dataset[0])):
        print("On the ", tree_level ,"th level of the REMOVAL tree")
        best_feature_to_remove = 0

        level_feature_set_accuracy = -1.0
        cur_feature_set = level_feature_set
        for feature in features_to_remove:
            cur_feature_set.remove(feature)
            cur_feature_set_accuracy = KCrossValidation(dataset, cur_feature_set)
            print("--Considering removing the ", feature, " feature with accuracy ", cur_feature_set_accuracy)
            if cur_feature_set_accuracy > level_feature_set_accuracy:
                level_feature_set_accuracy = cur_feature_set_accuracy
                best_feature_to_remove = feature
            cur_feature_set.append(feature)
        features_to_remove.remove(best_feature_to_remove)
        level_feature_set.remove(best_feature_to_remove)
        print("----Removed feature ", best_feature_to_remove)
        print("----Set is now ", level_feature_set, " with accuracy ", level_feature_set_accuracy)
        if level_feature_set_accuracy > best_feature_set_accuracy :
            best_feature_set_accuracy = level_feature_set_accuracy
            best_feature_set = level_feature_set[:]
    print("\nThe best set combination was ", best_feature_set, " with accuracy ", best_feature_set_accuracy)


# Custom Search
# Does a Forward Search, but also eliminates the feature with the worst accuracy 
# at each evaluation of considering which feature to add to the set
# This speeds up our search by not wasting time finding the distances with features
# that are likely to be irrelevant to our feature set
def CustomSearch(dataset):
    best_total_feature_set = []
    best_total_feature_set_accuracy = 0.0

    best_level_feature_set = []
    best_level_feature = 0
    best_level_feature_set_accuracy = 0.0

    current_feature_set = []
    current_feature_set_accuracy = 0.0
    
    features_to_consider = [i for i in range(1,len(dataset[0]))]
    tree_level = 0
    while len(features_to_consider) > 0:
        tree_level += 1
        print("On the ", tree_level ,"th level of the CUSTOM search tree")
        best_level_feature_set_accuracy = 0.0
        best_level_feature = 0
        worst_level_feature_set_accuracy = -1.0
        worst_level_feature = 0
        current_feature_set = best_level_feature_set
        for feature in features_to_consider:
            current_feature_set.append(feature)
            current_feature_set_accuracy = KCrossValidation(dataset, current_feature_set)
            print("--Considering adding the ", feature, " feature with accuracy ", current_feature_set_accuracy)
            if current_feature_set_accuracy > best_level_feature_set_accuracy :
                best_level_feature_set_accuracy = current_feature_set_accuracy
                best_level_feature = feature
            if current_feature_set_accuracy < worst_level_feature_set_accuracy or worst_level_feature_set_accuracy < 0.0 :
                worst_level_feature_set_accuracy = current_feature_set_accuracy
                worst_level_feature = feature
            current_feature_set.remove(feature)
        best_level_feature_set.append(best_level_feature)
        features_to_consider.remove(best_level_feature)
        if worst_level_feature in features_to_consider : features_to_consider.remove(worst_level_feature)
        print("----The best_level_feature_set is now ", best_level_feature_set, " with accuracy ", best_level_feature_set_accuracy)
        print("----Feature ", worst_level_feature, " has been removed from consideration. It had worst level set accuracy of ", worst_level_feature_set_accuracy)
        if best_level_feature_set_accuracy > best_total_feature_set_accuracy :
            best_total_feature_set_accuracy = best_level_feature_set_accuracy
            best_total_feature_set = best_level_feature_set[:]
    print("\nThe best set combination was ", best_total_feature_set, " with accuracy ", best_total_feature_set_accuracy)

# Normalizes the data
# Replaced entries of each feature with (x-avg(x))/std_dev(x)
def NormalizeData(dataset):
    for feature in range(1,len(dataset[0])) :
        sum = 0.0
        total_entries = 0.0
        for item in range(len(dataset)):
            sum += dataset[item][feature]
            total_entries += 1
        average = sum/total_entries
        
        summation_x_minus_xbar_squared = 0.0
        for item in range(len(dataset)):
            summation_x_minus_xbar_squared += math.pow((dataset[item][feature]-average), 2)

        standard_deviation = math.sqrt(summation_x_minus_xbar_squared/(total_entries-1))
        for item in range(len(dataset)):
            #print(dataset[item][feature], end="")
            dataset[item][feature] = (dataset[item][feature] - average)/standard_deviation
            #print (" is now ", dataset[item][feature])
    print("Dataset has been normalized")

############################  MAIN CODE  ########################################

# Open & Read File
#file = open('./cs_170_smallALL/cs_170_small33.txt','r')
file = open('./cs_170_largeALL/cs_170_large33.txt','r')
dataset = []

for line in file:
    list = [float(x) for x in line.split()]
    dataset.append(list)
NormalizeData(dataset)

#print ("\nCUSTOM SEARCH")
#CustomSearch(dataset)
print ("\n\n\nFORWARD SEARCH")
Forward_Search(dataset)
print ("\n\n\nBACKWARD SEARCH")
Backward_Search(dataset)