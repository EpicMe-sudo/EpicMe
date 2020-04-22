#Assume L is not empty
#Assume 0<n<=len(L)
#Sublists sould be without skipping elements in L

def getSublists(L,n):
    sublists=[] #sublists is list of lists so to use append() function, it must be declared as list at the front of definition
    for i in range(len(L)-n+1):#the number of sublists can be explained as the number of the possible last indexes in sublists which is n~Len(L)
        sublists.append(L[i:i+n])
    return sublists

test_list=[10,4,6,8,3,4,5,7,7,2]
n=4

sublists=getSublists(test_list,n)
print(sublists)

test_list=[1,1,1,1,4]
n=2

sublists=getSublists(test_list,n)
print(sublists)