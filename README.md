# endict


### 1. Description
英语词典 CLI 工具， 已命令行的形式提供基本的单词查询功能。

### 2. Dictionary Data Set

* [MySQL-English-Dictionary](https://sourceforge.net/projects/mysqlenglishdictionary/)

A dictionary with 176023 entries. Text was extracted from the files at http://www.mso.anu.edu.au/~ralph/OPTED/  and then parsed and stored in a 16MB MySQL database. The database has three fields : a. word b. wordtype and c. definition. You can use this standalone or as a jquery/ajax/PHP addon for your programs.

Acknowledgment of the original content:
* a. OPTED 
* b. Project Gutenburg 
* c. and the 1913 edition of Webster's Unabridged Dictionary


### 3. Double-Array-Trie Implementation

* [An Implementation of Double-Array Trie](https://linux.thai.net/~thep/datrie/datrie.html)
* [libdatrie-0.2.9](https://github.com/pytries/datrie/tree/master/libdatrie)


### 4. Implemenation References

* [MySQL/Client-C API](https://dev.mysql.com/doc/refman/5.7/en/c-api-building-clients.html)

* [Commandline Option Parser](https://developer.gnome.org/glib/2.26/glib-Commandline-option-parser.html)


### 5. ToDo

* 1. Using python-script convert data.csv to data.xml
* 2. Using libxml2 parsing data.xml
* 3. Implement "./endict --mode cli --dict ./dictionary.dat --data data.xml"



