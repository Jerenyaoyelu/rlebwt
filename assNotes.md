#Assignment Spec Analysis
--------------------------------------------------------------------------------------------------------------------
Task: 
create a search program that implements BWT backward search, which can efficiently search a RLFM encoded record file
--------------------------------------------------------------------------------------------------------------------
The original file (before RLFM) format is:
[<offset1>]<text1>[<offset2>]<text2>[<offset3>]<text3>... ...
where <offset1>, <offset2>, <offset3>, etc. are integer values that are used as unique record identifiers;
and <text1>, <text2>, <text3>, etc. are record values (text), which include any ASCII alphabets with ASCII values from 32 to 126, 
tab (ASCII 9) and newline (ASCII 10 and 13). For simplicity, there will be no open or close square bracket in the record values.
Example:
[3]Computers in industry[25]Data compression[33]Integration[40]Big data indexing[90]1990-02-19[190]20.55
--------------------------------------------------------------------------------------------------------------------
Task goals:
1. a command argument of either:
    - -m for the number of matching substrings (count duplicates),
        - given a query string, rlebwt will output the total number of matching substrings (count duplicates) to the standard output. The output is the total number, with an ending newline character.
    - -r for the number of unique matching records,
        - rlebwt will output the total number of unique matching records (do not count duplicates) 
    - -a for listing the identifiers of all the matching records (no duplicates and in ascending order), or
        - using the given query string, rlebwt will perform backward search on the given RLFM encoded file, and output the sorted and unique identifiers (no duplicates) of all the records that contain the input query string to the standard output. Each identifier is enclosed in a pair of square brackets, one line (ending with a '\n') for each match. 
    - -n for displaying the record value of a given record identifier;
        - using the given record identifier, rlebwt will output the original record value (text) to the standard output with a '\n' at the end.
2. the path to a RLFM encoded file (without its file extension);
3. the path to a index folder; and
4. a quoted query string (i.e., the search term) for option -m, -r, -a, or a quoted record identifier for option -n
--------------------------------------------------------------------------------------------------------------------
Sample files instuction:
The file extensions represent their corresponding types:
- FILENAME.txt - the original text file. It is provided for your reference only. It will not be available during auto marking.
- FILENAME.s - corresponds to S in the RLFM lecture slides and its original paper. It is the BWT text with the consecutive duplicates removed.
- FILENAME.b - corresponds to the bit array B in the RLFM lecture slides and its original paper. It is in binary format, which can be inspected using xxd as shown later.
- FILENAME.bb - corresponds to the bit array B' in the RLFM lecture slides and its original paper. It is in binary format, which can be inspected using xxd as shown later. This file is not provided during auto marking. Your rlebwt will need to generate it.
Attention:
For the B and B' arrays, after the last bit is written to the file, fill in the gap (if any) of the last byte with bit 1. Check the xxd examples below for details.
--------------------------------------------------------------------------------------------------------------------
Initialization and External Files:
    rlebwt -X FILENAME INDEX_FOLDER QUERY_STRING
    where X can be any one of the options (-m, -r, -a, -n), it will take FILENAME.s and FILENAME.b as input; and also check if FILENAME.bb exists. If FILENAME.bb does not exist, it will generate one. 
    After that, it will check if INDEX_FOLDER exists. If not, it will create it as an index folder. Index files will then be generated inside this index folder accordingly.

In addition to the B' array, your solution is allowed to write out up to 6 external index files that are in total no larger than the total size of the given, input FILENAME.s file plus 2 x the size of the given FILENAME.b. If your index files are larger than this limit, you will receive zero points for the tests that involve that given FILENAME. You may assume that the index folder (and its index files inside) will not be deleted during all the tests for a given FILENAME, and all the INDEX_FOLDER are uniquely and correspondingly named. Therefore, to save time, you only need to generate the index files when their folder does not exist yet.
--------------------------------------------------------------------------------------------------------------------