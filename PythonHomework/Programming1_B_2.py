#Assume list L is not empty

#Version1. returns only the number of longestRun
def longestRun_ver1(L):
    tmp=L[0]
    num_check=1
    check_list=[]
    for i in range(1,len(L)):
        if L[i]<tmp:
            tmp=L[i]
            check_list.append(num_check)
            num_check=1
        elif i==len(L)-1:
            check_list.append(num_check+1)
        else:
            num_check+=1
    return max(check_list)

def max_value_dict(mydict):
   key_tmp=0
   for key in mydict:
        if key_tmp==0 or mydict[key]>mydict[key_tmp]:
            key_tmp=key
   return key_tmp
        

#Version2. returns sublist of the longestRun
def longestRun_ver2(L):
    tmp=L[0]
    key=0
    check_dict={}
    for i in range(1,len(L)):
        if L[i]<tmp:
            tmp=L[i]
            key=i
            check_dict[key]=1
        else:
            check_dict[key]+=1
    
    key=max_value_dict(check_dict)
    
    return L[key:key+check_dict[key]]

L=[10,4,6,8,3,4,5,7,7,2]

print("longestRun_ver1 return: ",longestRun_ver1(L))

print("\n\n")

print("longestRun_ver2 return: ",longestRun_ver2(L))
