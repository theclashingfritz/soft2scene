// soft2scene.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <assert.h>
#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <iostream>
#include <Windows.h>
#include <vector>

#include <SAA.h>

#include "Softimage\SAA_interface.h"

//// Global Variables ////

// Global strings
static char default_scene_name[] = "suitA-zero.1-0";
static char *scene_name = default_scene_name;
static char *model_name = nullptr;
static char default_database_name[] = "C:/Users/theclashingfritz/player/softimage";
static char *database_name = default_database_name;
static char default_rsrc_path[] = "D:/Softimage/SOFT3D_4.0/3D/rsrc";
static char *rsrc_path = default_rsrc_path;
static char *texture_list_filename = nullptr;

// Global SAA variables
static SAA_Database database;
static SAA_Scene scene;
static SAA_Boolean uv_swap = FALSE;

// Global toggles.
static bool make_poly = true;
static bool make_nurbs = true;
static bool make_duv = false;
static bool use_prefix = false;

// Other global variables. 
static std::ofstream texture_list_file;
static int pose_frame = 0;
static int nurbs_step = 1;
static int num_tex_loc = 0;
static int num_tex_glb = 0;

// Constants
static const int TEX_PER_MAT = 1;

void IndentStream(std::stringstream &ss, int indent_level) {
    for (int indent = 0; indent < indent_level; indent++) {
        ss << ' ';
    }
}

int ProcessScene(SAA_Database *database, SAA_Scene *scene, const char *scene_name) {
    SI_Error error = SAA_sceneLoad(database, scene_name, scene);
    if (error != SI_ERR_NONE) {
        fprintf(log_file, "Failed to load scene %s with error: %d\n", scene_name, error);
        safe_exit(1);
    }

    // Open an output file for texture list if specified.
    if (texture_list_filename != NULL) {
        _unlink(texture_list_filename);
        texture_list_file.open(texture_list_filename, std::ios::out, 0666);

        if (!texture_list_file) {
            fprintf(log_file, "Couldn't open output file: %s\n", texture_list_filename);
            safe_exit(1);
        }
    }

    if (SAA_updatelistGet(scene) == SI_SUCCESS) {
        float time;

        fprintf(log_file, "Setting Scene to frame %d...\n", pose_frame);
        // SAA_sceneSetPlayCtrlCurrentFrame( &scene, pose_frame );
        SAA_frame2Seconds(scene, pose_frame, &time);
        SAA_updatelistEvalScene(scene, time);
        //Sleep(100);
        SAA_updatelistEvalScene(scene, time);
        if (false) { // make_pose
            SAA_sceneFreeze(scene);
        }
    }
    
    // Open our binary file for writing.
    std::string filename(scene_name);
    {
        std::string file_ext(".siscene");
        filename += file_ext;
    }
    dprintf("Writing scene '%s' to file '%s'.\n", scene_name, filename.c_str());
    CompressedBinaryFile file(filename.c_str());

    return 0;
}

SI_Error init_soft2scene(int argc, char* argv[]) {
    SI_Error error = SI_SUCCESS;

    std::cout << "Running soft2scene!\n";

    if ((scene_name == nullptr && model_name == nullptr) || database_name == nullptr) {
        //Usage();
        safe_exit(1);
    }
    
    if ((error = SAA_Init(rsrc_path, FALSE)) != SI_SUCCESS) {
        printf("Error: Couldn't get resource path!\n");
        safe_exit(1);
    }
    
    if ((error = SAA_databaseLoad(database_name, &database)) != SI_SUCCESS) {
        printf("Error: Couldn't load database!\n");
        safe_exit(1);
    }

    if ((error = SAA_sceneGetCurrent(&scene)) != SI_SUCCESS) {
        printf("Error: Couldn't load current scene!\n");
        safe_exit(1);
    }

    // Open the log for debugging.
    errno_t err = fopen_s(&log_file, "soft2scene.log", "w+");

    // Process the scene.
    if (scene_name != NULL) {
        ProcessScene(&database, &scene, scene_name);
    } else if (model_name != NULL) {

    }

    // Close the log for debugging.
    fclose(log_file);

    return error;
}


extern "C" {

/**
 * Instantiate converter and process a scenr
 */
__declspec(dllexport) SI_Error soft2scene(int argc, char *argv[]) {
#ifdef _DEBUG
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    flags |= _CRTDBG_CHECK_ALWAYS_DF;
    flags |= _CRTDBG_CHECK_CRT_DF;
    _CrtSetDbgFlag(flags);
#endif

    // Pass control to the c++ system
    return init_soft2scene(argc, argv);
}

}