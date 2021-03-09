#include "bucket.h"

struct lnode {
	struct lnode * next;
	char * key;
};

struct list {
	struct lnode * start;
	struct lnode * end;
};

struct subdirectory {
    char * path; // Path to current subdirectory
    struct list * date_files;
    struct RBTree * tree; // Tree with all patients of country
    struct bucket ** diseaseHashtable;
    struct subdirectory * next; // Pointer to next subdirectory_info
};

struct subdirectory_list {
    struct subdirectory * start;
    struct subdirectory * end;
};


///////////////////////////////////////////   list-lnode   ///////////////////////////////////////////

struct list * new_list();

struct lnode * new_lnode(char * date);

void insert_lnode(struct list * list, struct lnode * lnode);

int delete_key_list(struct list * list, char * key);

void delete_list(struct list * list);

void print_list(struct list * list);

// returns 1 if key exist in list else 0
int key_exist(struct list * list, char * key);


///////////////////////////////////////////   subdirectory-subdirectory_list   ///////////////////////////////////////////

struct subdirectory * new_subdirectory(char * path, int diseaseHashtableNumOfEntries);

void insert_subdirectory(struct subdirectory_list * subdirectory_list, struct subdirectory * subdirectory);

void insert_date_file(struct subdirectory * subdirectory, char * date);

struct subdirectory_list * new_subdirectory_list();

void delete_subdirectory_list(struct subdirectory_list * subdirectory_list, int diseaseHashtableNumOfEntries);

void delete_hashtables(struct bucket ** diseaseHashtable, int diseaseHashtableNumOfEntries);
