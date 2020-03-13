# C_dictionary
Use the C language to realize the Python dictionary features

This repo uses C language to realize some basic features of the Python dictionary. Main knowledge includes hash function and collision chains. 

Features include 
"create": create a dictionary with a constant start size,
"destroy": destroy the dictionary (for computer memory consideration),
"insert/update": insert/update key-value pairs, the two features are written in the same function,
"delete": delete a key-value pairs, also you can delete a non-exisiting key, simply no work is done in the program,
"search": search the value for a input key, return NULL if the key is not existing,
"status": this is an extra function, to keep track of the dic size, dic item counts and the dic load factor (which is used during up and down sizing)
"view_pairs": print out all key-value pairs.

The functions set up an array with a start size of 30. Once the load factor increases to 70%, the dictionary undergoes upsizing, so that the size becomes 60. Once the load factor decreases to 20%, the dictionary undergoes downsizing, the size becomes max(current size / 2, 30).

Each key is calculated using a hash function, sum(key[i]*p^(i-1)), the sum is mod against the size, to return the index of the location. If the location is occupied already, the function tracks down the array to find the first un-occupied slot, or sometimes called bucket. 

The makefile creates a simple test program, that extracts some english letters and implement the basic functions. 

Reference:
I referenced several files to get an overall picture of how the hash table and the dictionary works. Some main websites and information are listed below.

https://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)HashTables.html?highlight=%28CategoryAlgorithmNotes%29

https://github.com/jamesroutley/write-a-hash-table.git 

http://cseweb.ucsd.edu/~kube/cls/100/Lectures/lec16/lec16.html

https://github.com/first20hours/google-10000-english.git 
