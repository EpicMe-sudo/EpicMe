def dict_invert(immutable_dictionary):
    inverse_dictionary={}
    key_of_invdict_mustbesorted=[]
    for key in immutable_dictionary:
        if immutable_dictionary[key] in inverse_dictionary: #this line is for duplicated keys 
            key_of_invdict_mustbesorted.append(immutable_dictionary[key])
            inverse_dictionary[immutable_dictionary[key]].append(key)
        else:
            inverse_dictionary[immutable_dictionary[key]]=[key]
    key_of_invdict_mustbosorted=list(set(key_of_invdict_mustbesorted)).sort()#this line is unnecessary but for arranagement
    for key in key_of_invdict_mustbesorted:
        inverse_dictionary[key].sort
    return inverse_dictionary

test_dict={1:10,2:20,3:30,4:10,5:20,6:60}

test_invdict=dict_invert(test_dict)

print("This is test dict: ",test_dict)
print("\n\n")
print("This is test invdict",test_invdict)