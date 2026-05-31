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

#include "binaryFile.h"
#include "util.h"

#include "Softimage\SI_interface.h"

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
static SAA_Boolean uv_swap = FALSE;

// Global SI variables
static SI_Scene siscene;

// Global toggles.
static bool make_poly = true;
static bool make_nurbs = true;
static bool make_duv = false;
static bool make_pose = false;
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
    
    // Convert the scene into our new format.
    siscene.saa_scene = scene;
    error = Scene_SAA2SI(siscene);
    if (error != SI_ERR_NONE) {
        fprintf(log_file, "Failed to convert scene %s with error: %d\n", scene_name, error);
        safe_exit(1);
    }
    
    fprintf(log_file, "Saving converted scene %s...\n", siscene.name);
    
    fprintf(log_file, "DEBUG: Scene prefix: %s\n", siscene.prefix);
    fprintf(log_file, "DEBUG: Scene name: %s\n", siscene.name);
    fprintf(log_file, "DEBUG: Scene texture2d count: %d\n", siscene.textures2d.size());
    fprintf(log_file, "DEBUG: Scene texture3d count: %d\n", siscene.textures3d.size());
    fprintf(log_file, "DEBUG: Scene material count: %d\n", siscene.materials.size());
    fprintf(log_file, "DEBUG: Scene model count: %d\n", siscene.models.size());
    fprintf(log_file, "DEBUG: Scene fcurve count: %d\n", siscene.fcurves.size());
    fprintf(log_file, "DEBUG: Scene constraint count: %d\n", siscene.constraints.size());
    fprintf(log_file, "DEBUG: Scene scaling type: %d\n", siscene.scaling);
    
    fprintf(log_file, "====TEXTURE2D====\n");
    for (uint32_t i = 0; i < siscene.textures2d.size(); i++) {
        SI_Texture2d &tex = siscene.textures2d[i];
        fprintf(log_file, "DEBUG [TEXTURE2D]: %d, %d, %d, %s.%s, %s\n", i, tex.chapter, tex.revision, tex.prefix, tex.name, tex.filepath);
    }
    
    fprintf(log_file, "====TEXTURE3D====\n");
    for (uint32_t i = 0; i < siscene.textures3d.size(); i++) {
        SI_Texture3d &tex = siscene.textures3d[i];
        fprintf(log_file, "DEBUG [TEXTURE3D]: %d, %d, %d, %s.%s\n", i, tex.chapter, tex.revision, tex.prefix, tex.name);
    }
    
    fprintf(log_file, "====MATERIAL====\n");
    for (uint32_t i = 0; i < siscene.materials.size(); i++) {
        SI_Material &mat = siscene.materials[i];
        fprintf(log_file, "DEBUG [MATERIAL]: %d, %d, %d, %s.%s, %d, %d, %d, %d\n", i, mat.chapter, mat.revision, mat.prefix, mat.name, 
                mat.num_active_tex2d, mat.num_passive_tex2d, mat.num_active_tex3d, mat.num_passive_tex3d);
    }
    
    fprintf(log_file, "====FCURVES====\n");
    for (uint32_t i = 0; i < siscene.fcurves.size(); i++) {
        SI_FCurve &fcurve = siscene.fcurves[i];
        fprintf(log_file, "DEBUG [FCURVE]: %d, %d, %d, %s.%s, %s\n", i, fcurve.chapter, fcurve.revision, fcurve.prefix, fcurve.name, fcurve.trackname);
    }
    
    // Open our binary file for writing.
    std::string filename(scene_name);
    {
        std::string file_ext(".siscene");
        filename += file_ext;
    }
    dprintf("Writing scene '%s' to file '%s'.\n", scene_name, filename.c_str());
    BinaryFile file(filename.c_str());
    
    file.write("SISCENE");
    SI_WriteScene(siscene, &file);
    
    //file.compress_file();

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
    
    SAA_Database database;
    if ((error = SAA_databaseLoad(database_name, &database)) != SI_SUCCESS) {
        printf("Error: Couldn't load database!\n");
        safe_exit(1);
    }
    
    SAA_Scene scene;
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