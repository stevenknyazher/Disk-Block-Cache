#include <stdio.h>
#include <stdlib.h>

#define LOCKED 0
#define LRU 1

struct node {
  int id;
  int stats;
  struct node *hash_next;
  struct node *list_prev;
  struct node *list_next;
};

typedef struct node block_type;

block_type *locked_list = NULL;
block_type *lru_list = NULL;
block_type **hash_table = NULL;

int num_rows;

void initialize() {
  printf("Enter number of indices for hash table: ");
  scanf("%d", &num_rows);

  hash_table = (block_type**)malloc(num_rows * sizeof(block_type*));

  for (int i = 0; i < num_rows; i++) {
    hash_table[i] = NULL;
  }
  return;
}

void print_hash_table() {
  block_type *current_block_ptr;
  printf("\nIndex\tID\tStatus\tPrev\tNext\n");
  printf("------------------------------------\n");
  for (int i = 0; i < num_rows; i++) {
    current_block_ptr = hash_table[i];
      while (current_block_ptr != NULL) {
      printf("%d\t%d\t", i, current_block_ptr -> id);
      if (current_block_ptr -> stats == LOCKED) {
        printf("Locked");
      } else {
        printf("LRU");
      }
      if (current_block_ptr -> list_prev == NULL || current_block_ptr -> stats == LOCKED) {
        printf("\t ");
      } else {
        printf("\t%d", current_block_ptr -> list_prev -> id);
      }	
      if (current_block_ptr -> list_next == NULL) {
        printf("\t \n");
      } else {
        printf("\t%d\n", current_block_ptr -> list_next -> id);
      }
      current_block_ptr = current_block_ptr -> hash_next;
    }
  }
  return;
}

void insert_access_block() {
  int row;
  int my_id;
  int my_stat;

  block_type *new_block_ptr;
  block_type *current_block_ptr;
  block_type *found_block_ptr = NULL;
  block_type *temp_prev;
  block_type *temp_next;

  printf("Enter block id: ");
  scanf("%d", &my_id);

  printf("Enter status (0 = LOCKED, 1 = LRU): ");
  scanf("%d", &my_stat);

  row = my_id / num_rows;

  if (hash_table[row] == NULL) {
    new_block_ptr = (block_type*)malloc(sizeof(block_type));
    new_block_ptr -> id = my_id;
    new_block_ptr -> stats = my_stat; 
    new_block_ptr -> hash_next = NULL;
    new_block_ptr -> list_prev = NULL;
    new_block_ptr -> list_next = NULL;
    hash_table[row] = new_block_ptr;
  } else { 
    current_block_ptr = hash_table[row]; 

    if ((current_block_ptr -> id == my_id) && (current_block_ptr -> hash_next == NULL)) {
      found_block_ptr = current_block_ptr;
    }
    while (current_block_ptr -> hash_next != NULL) {
      if (current_block_ptr -> id == my_id) {
        found_block_ptr = current_block_ptr;
        break;
      }
      current_block_ptr = current_block_ptr -> hash_next;
    }
    if (current_block_ptr -> id == my_id) {
      found_block_ptr = current_block_ptr;
    }
    if (found_block_ptr == NULL) {
      new_block_ptr = (block_type*)malloc(sizeof(block_type));
      new_block_ptr -> id = my_id;
      new_block_ptr -> stats = my_stat;
      new_block_ptr -> hash_next = NULL;
      new_block_ptr -> list_prev = NULL;
      new_block_ptr -> list_next = NULL;
      current_block_ptr -> hash_next = new_block_ptr;
    }
  }
  if (my_stat == LOCKED) {
    if (found_block_ptr != NULL) {
      return;
    }
    if (locked_list == NULL) {
      locked_list = new_block_ptr;
      return;
    }
    current_block_ptr = locked_list;
    while (current_block_ptr -> list_next != NULL) {
      current_block_ptr = current_block_ptr -> list_next;
    }
    current_block_ptr -> list_next = new_block_ptr;
    new_block_ptr -> list_prev = current_block_ptr;
    return;
  } else {
    if (lru_list == NULL) {
      lru_list = new_block_ptr;
      return;
    }
    current_block_ptr = lru_list;
    while (current_block_ptr -> list_next != NULL) {
      current_block_ptr = current_block_ptr -> list_next;	
    }
    if (found_block_ptr == NULL) {
      current_block_ptr -> list_next = new_block_ptr;	
      new_block_ptr -> list_prev = current_block_ptr;
      return;
    }
    if (lru_list -> id == my_id) {
      if (lru_list -> list_next == NULL) {
        return;
      }
      lru_list = lru_list -> list_next;
      lru_list -> list_prev = NULL;
      current_block_ptr -> list_next = found_block_ptr;	
      found_block_ptr -> list_prev = current_block_ptr;
      found_block_ptr -> list_next = NULL;
      return;
    }

    if (current_block_ptr -> id == my_id) {
      return;
    }
    temp_prev = found_block_ptr -> list_prev;
    temp_next =  found_block_ptr -> list_next;
    temp_prev -> list_next = temp_next;
    temp_next -> list_prev = temp_prev;
    current_block_ptr -> list_next = found_block_ptr;
    found_block_ptr -> list_prev = current_block_ptr;
    found_block_ptr -> list_next = NULL;
    printf("Block %d added to head of LRU list\n", my_id);
  }
  return;
} 

void delete_block() {
  int my_id;

  block_type *current_block_ptr;
  block_type *found_block_ptr;
  block_type *prev_hash;
  block_type *temp_next;
  block_type *temp_prev;

  printf("Enter block id: ");
  scanf("%d", &my_id);

  int row = my_id / num_rows;

  if (hash_table[row] == NULL) {
    return;
  }
  current_block_ptr = hash_table[row];
  if (current_block_ptr -> id == my_id) {
    found_block_ptr = current_block_ptr;
    hash_table[row] = current_block_ptr -> hash_next;
  } else {
    prev_hash = current_block_ptr;
    while (current_block_ptr -> hash_next != NULL) {
      if (current_block_ptr -> id == my_id) {
        found_block_ptr = current_block_ptr;
        break;
      }
      prev_hash = current_block_ptr;
      current_block_ptr = current_block_ptr -> hash_next;
    }
    if (current_block_ptr -> id == my_id) {
      found_block_ptr = current_block_ptr;
      prev_hash -> hash_next = NULL;
    }
  }
  if (found_block_ptr == NULL) {
    return;
  }
  if (lru_list -> id == my_id) {
    lru_list = lru_list -> list_next;
    if (lru_list != NULL) {
      lru_list -> list_prev = NULL; 
    } 
    free(found_block_ptr);
    return;
  }
  temp_prev = found_block_ptr -> list_prev;
  temp_next =  found_block_ptr -> list_next;
  temp_prev -> list_next = temp_next;
  if (temp_next != NULL) {
    temp_next -> list_prev = temp_prev;
  }
  free(found_block_ptr);
  return;
}

void quit() {
  if (hash_table != NULL) {
    free(hash_table);
  }
  return;
}

int main() {
  int choice = 0;
  while(choice != 4) {
    printf("\n\nDisk Block Cache\n");
    printf("----------------");
    printf("\n1) Enter parameters");
    printf("\n2) Insert/access disk block in cache");
    printf("\n3) Delete disk block from cache");
    printf("\n4) Quit program\n");

    printf("\nEnter a selection: ");
    scanf("%d", &choice);

    switch(choice) {
      case 1:
        initialize();
        break;
      case 2:
        insert_access_block();
        print_hash_table();
        break;
      case 3:
        delete_block();
        print_hash_table();
        break;
      case 4:
        quit();
        break;
      default: 
        printf("Invalid option\n");
        break;
    }
  }
  return 1;
}
