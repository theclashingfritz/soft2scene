#pragma once

#include <SAA.h>
#include <Windows.h>

#include "Element.h"
#include "Types.h"

#ifdef _DEBUG
  #define dprintf(format, ...) printf(format, __VA_ARGS__);
#else
  #define dprintf(format, ...)
#endif

#define lprintf(format, verbose_level, ...) if (verbose >= verbose_level) { printf(format, __VA_ARGS__); }
#define lfprintf(file, format, verbose_level, ...) if (verbose >= verbose_level) { fprintf(file, format, __VA_ARGS__); }

extern int verbose;
extern FILE *log_file;

// General

void safe_exit(int code);
int *MakeIndexMap(int *indices, int num_indices, int map_size);


// General (Templates)

template <typename T, typename S>
void ArrayRemove(T *&arr, size_t index, S &size) {
	for (S i = index; i < size - 1; i++) {
		arr[i] = arr[i + 1];
    }
	size--;
}

template <typename T, typename S>
void ArrayRemove(T **&arr, size_t index, S &size) {
	for (S i = index; i < size - 1; i++) {
		arr[i] = arr[i + 1];
    }
	size--;
}

/**
  * Remove all duplicates in the array and return a index map. (The index map is for mapping the old indicies to the new ones.)
  */
template <typename T, typename S>
S *RemoveDuplicates(T *&arr, S &size) {
	// Allocate and initialize our index map.
    S *index_map = new S[size + 1];
    memset(index_map, 0, size);
    
	for (S i = 0; i < size; i++) {
		T &item = arr[i];
        index_map[i] = i;
		for (S j = i + 1; j < size; j++) {
			if (memcmp(&item, &arr[j], sizeof(T)) == 0) {
				ArrayRemove(arr, j, size); 
                index_map[j] = i;
                j--;
			}
		}
	}

    return index_map;
}


/**
  * Remove all duplicates in the array and return a index map. (The index map is for mapping the old indicies to the new ones.)
  */
template <typename T, typename S>
S *RemoveDuplicates(T **arr, S &size) {
	// Allocate and initialize our index map.
    S *index_map = new S[size + 1];
    memset(index_map, 0, size);
    
	for (S i = 0; i < size; i++) {
		T *&item = arr[i];
        index_map[i] = i;
		for (S j = i + 1; j < size; j++) {
			if (item == arr[j]) {
				ArrayRemove(arr, j, size);
                index_map[j] = i;
                j--;
			}
		}
	}

    return index_map;
}

// SAA

char *GetName(SAA_Scene *scene, SAA_Elem *element);
char *GetPrefix(SAA_Scene *scene, SAA_Elem *element);
char *GetFullName(SAA_Scene *scene, SAA_Elem *element);
char *GetTextureName(SAA_Scene *scene, SAA_Elem *texture);
char *GetTextureFilepath(SAA_Scene *scene, SAA_Elem *texture);

/**
  * Remove all duplicates in the array and return a index map. (The index map is for mapping the old indicies to the new ones.)
  */
template <typename S>
S *RemoveDuplicates(SAA_Scene *scene, SAA_Elem *&arr, S &size) {
	// Allocate and initialize our index map.
	S *index_map = new S[size + 1];
	memset(index_map, 0, size);

	for (S i = 0; i < size; i++) {
		SAA_Elem &item = arr[i];
		index_map[i] = i;
        char *name1 = GetName(scene, &item);
        
		for (S j = i + 1; j < size; j++) {
            SAA_Elem &item2 = arr[j];
			char *name2 = GetName(scene, &item2);

			if (memcmp(&item, &item2, sizeof(SAA_Elem)) == 0 && strcmp(name1, name2) == 0) {
                char *elem_name = GetName(scene, arr);
                printf("DEBUG: Removing duplicate element %s from array.\n", elem_name);
                
				ArrayRemove(arr, j, size);
				index_map[j] = i;
				j--;
			}

			delete[] name2;
		}
        
        delete[] name1;
	}

	return index_map;
}