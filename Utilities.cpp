#include "Utilities.h"

int verbose = 3;
FILE *log_file = nullptr;

// General

void safe_exit(int code) {
    // Close the log for debugging.
    if (log_file) {
        fflush(log_file);
        fclose(log_file);
        log_file = nullptr;
    }

    exit(code);
}

int *MakeIndexMap(int *indices, int num_indices, int map_size) {
    int i, j;

    // Allocate map array
    int *map = new int[map_size];

    if (map != NULL) {
        for (i = 0; i < map_size; i++) {
            j = 0;
            int found = 0;
            while (j < num_indices) {
                if (indices[j] == i) {
                    map[i] = j;
                    lfprintf(log_file, "map[%d] = %d\n", 2, i, map[i]);
                    found = 1;
                    break;
                }
                j++;
            }
            if (!found) {
                lfprintf(log_file, "WARNING: Orphan vertex (%d)\n", 2, i);
                // default to -1 for now
                map[i] = -1;
            }
        }
    } else {
        fprintf(log_file, "Not enough memory for index map...\n");
    }

    return map;
}


// SAA

char *GetName(SAA_Scene *scene, SAA_Elem *element) {
    int name_len = 0;

    // Get the name
    SAA_elementGetNameLength(scene, element, &name_len);
    char* name = new char[++name_len];
    SAA_elementGetName(scene, element, name_len, name);
    name[--name_len] = 0;

    return name;
}


char *GetPrefix(SAA_Scene *scene, SAA_Elem *element) {
    int prefix_len = 0;

    // Get the prefix
    SAA_elementGetPrefixLength(scene, element, &prefix_len);
    char* prefix = new char[++prefix_len];
    SAA_elementGetPrefix(scene, element, prefix_len, prefix);
    prefix[--prefix_len] = 0;

    return prefix;
}

char *GetFullName(SAA_Scene *scene, SAA_Elem *element) {
    char *prefix = GetPrefix(scene, element);
    char *name = GetName(scene, element);

    // Construct the full name from both the prefix and name.
    size_t name_len = strlen(name);
    size_t prefix_len = strlen(prefix);
    int fullname_len = name_len + prefix_len + 1;
    char *fullname = new char[fullname_len + 1];
    strncpy_s(fullname, fullname_len, prefix, prefix_len);
    strncpy_s(fullname + prefix_len + 1, fullname_len - prefix_len, name, name_len);
    fullname[prefix_len] = '-';

    delete[] name;
    delete[] prefix;

    return fullname;
}

char *GetTextureName(SAA_Scene *scene, SAA_Elem *texture) {
    char *name = nullptr;
    int filename_len = 0;

    // Get the textures name.
    SAA_texture2DGetPicNameLength(scene, texture, &filename_len);

    if (filename_len) {
        filename_len += 5;
        char *filename = new char[filename_len];
        memset(filename, 0, filename_len);
        SAA_texture2DGetPicName(scene, texture, filename_len, filename);
        
        char *tmpName = NULL;
        tmpName = strrchr(filename, '/');
        if (tmpName) {
            tmpName++;
            name = new char[strlen(tmpName) + 5];
            strcpy_s(name, strlen(tmpName) + 5, tmpName);
            
            delete[] filename;
        } else {
            name = filename;
        }
    }

    // make sure we are not being passed a NULL image, an empty image string or
    // the default image created by egg2soft
    if ((name != nullptr) && strlen(name) && (strstr(name, "noIcon") == nullptr)) {
        return name;
    } else {
        fprintf(log_file, "Warning: GetTextureName received NULL name.\n");
        return nullptr;
    }
}

char *GetTextureFilepath(SAA_Scene *scene, SAA_Elem *texture) {
    char *filepath = nullptr;
    int filepath_len = 0;

    // Get the textures name.
    SAA_texture2DGetPicNameLength(scene, texture, &filepath_len);

    if (filepath_len) {
        filepath_len += 5;
        filepath = new char[filepath_len];
        memset(filepath, 0, filepath_len);
        SAA_texture2DGetPicName(scene, texture, filepath_len, filepath);
        int err = strcat_s(filepath, filepath_len, ".pic");
    }

    // make sure we are not being passed a NULL image, an empty image string or
    // the default image created by egg2soft
    if ((filepath != nullptr) && strlen(filepath) && (strstr(filepath, "noIcon") == nullptr)) {
        return filepath;
    } else {
        fprintf(log_file, "Warning: GetTextureFilepath received NULL filepath.\n");
        return nullptr;
    }
}