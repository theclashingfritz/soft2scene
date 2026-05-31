#ifndef DYNARRAY_H
#define DYNARRAY_H

#include "types.h"

#include <algorithm>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

template <typename T>
class DynArray {
    public:
        inline DynArray() {
            items = nullptr;
            nb_items = 0;
        }
          
        inline DynArray(size_t count) {
            items = (T *)calloc(count, sizeof(T));
            assert(items != NULL);
            nb_items = count;
        }
        
        inline ~DynArray() {
            clear();
        }
        
        inline T &at(size_t index) {
            assert(index < nb_items);
            return items[index];
        }

        inline T at(size_t index) const {
            assert(index < nb_items);
            return items[index];
        }
        
        inline void clear() {
            if (items != nullptr) { free(items); }
            items = nullptr;
            nb_items = 0;
        }
        
        inline T *data() {
            return items;
        }
        
        inline const T *data() const {
            return items;
        }
        
        size_t resize(size_t size) {
            // Any size at or below 0 is a empty array.
            // So make sure to deallocate if we have too.
            if (size <= 0) {
                clear();
                return 0;
            }
            
            // Check for if we've already allocated items or not.
            if (items != nullptr) {
                // Resize the existing memory block for our items.
                items = (T *)realloc(items, size * sizeof(T));
                // Only pre-initialize the new memory if the new size is bigger then the old one.
                if (nb_items < size) {
                    size_t d_sz = size - nb_items;
                    memset(items + (nb_items * sizeof(T)), 0, d_sz * sizeof(T));
                }
            } else {
                items = (T *)calloc(size, sizeof(T));
            }
            nb_items = size;
            return nb_items;
        }
        
        inline size_t push_back(const T &item) {
            size_t index = nb_items;
            reserve(nb_items + 1);
            items[index] = item;
            return index;
        }
        
        inline size_t size() { return nb_items; }
        
        inline T &operator [](size_t index) {
            return items[index];
        }

        inline T operator [](size_t index) const {
            return items[index];
        }
        
    private:
        // Reserve is indentical to resize but it doesn't pre-initialize
        // any newly allocated items.
        size_t reserve(size_t size) {
            // Any size at or below 0 is a empty array.
            // So make sure to deallocate if we have too.
            if (size <= 0) {
                clear();
                return 0;
            }
            
            // Check for if we've already allocated items or not.
            if (items != nullptr) {
                // Resize the existing memory block for our items.
                items = (T *)realloc(items, size * sizeof(T));
            } else {
                items = (T *)calloc(size, sizeof(T));
            }
            nb_items = size;
            return nb_items;
        }
        
        T *items;
        size_t nb_items;
};


#endif // DYNARRAY_H