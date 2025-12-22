#include "SI_interface.h"

#include <assert.h>
#include <cstring>

SI_Error SI_GetElement(SAA_Scene *scene, SAA_Elem *elem, SI_Element &si_elem) {
    if (scene == nullptr || elem == nullptr) { return SI_ERR_BAD_ARGUMENT; }
    
    // Verify and make sure our element is valid.
    SAA_Boolean valid = TRUE;
    SAA_elementIsValid(scene, elem, &valid);
    if (valid == FALSE) { return SI_ERR_BAD_ELEMENT; }
    
    // Get the length of our elements prefix.
    int prefixlength = -1;
    SAA_elementGetPrefixLength(scene, elem, &prefixlength);
    si_elem.prefix_len = prefixlength;
    
    // Get the prefix of our element.
    si_elem.prefix = new char[++prefixlength]();
    SAA_elementGetPrefix(scene, elem, prefixlength, si_elem.prefix);
    
    // Get the length of our elements name.
    int namelength = -1;
    SAA_elementGetNameLength(scene, elem, &namelength);
    si_elem.name_len = namelength;
    
    // Get the name of our element.
    si_elem.name = new char[++namelength]();
    SAA_elementGetName(scene, elem, namelength, si_elem.name);

    // Get the reivision of the element.
    SAA_elementGetRevision(scene, elem, &si_elem.revision);

    // Get the chapter for the Element. 
    // This is good for sorting and resolving the elements type later on if we need too.
    SAA_elementGetChapter(scene, elem, &si_elem.chapter);

    // Get the wireframe info.
    SAA_elementGetWireColor(scene, elem, &si_elem.wireframecol);
    SAA_elementGetWireType(scene, elem, &si_elem.wiretype);
    
    si_elem.id = elem->elemid;
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_GetTexture2d(SAA_Scene *scene, SAA_Elem *elem, SI_Texture2d &tex) {
    SI_Error error = SI_GetElement(scene, elem, tex);
    if (error != SI_SUCCESS) { return error; }
    
    // Get the length of the pic filepath.
    int picfilepathlen = -1;
    error = SAA_texture2DGetPicNameLength(scene, elem, &picfilepathlen);
    tex.filepath_len = picfilepathlen;
    
    // Get the pic filepath.
    tex.filepath = new char[++picfilepathlen];
    error = SAA_texture2DGetPicName(scene, elem, picfilepathlen, tex.filepath);
    
    error = SAA_texture2DGetTransparency(scene, elem, &tex.transparency);
    
    error = SAA_texture2DGetAmbient(scene, elem, &tex.ambieance);
    
    error = SAA_texture2DGetRoughness(scene, elem, &tex.roughness);
    
    error = SAA_texture2DGetRotation(scene, elem, &tex.rotation);
    
    error = SAA_texture2DGetSpecular(scene, elem, &tex.specular);
    
    error = SAA_texture2DGetDiffuse(scene, elem, &tex.diffuse);
    
    error = SAA_texture2DGetBlending(scene, elem, &tex.blend);
    
    error = SAA_texture2DGetReflection(scene, elem, &tex.reflection);
    
    error = SAA_texture2DGetRefMap(scene, elem, &tex.reflection_int);
    
    error = SAA_texture2DGetUScale(scene, elem, &tex.u_scale);
    error = SAA_texture2DGetVScale(scene, elem, &tex.v_scale);
    
    error = SAA_texture2DGetUOffset(scene, elem, &tex.u_offset);
    error = SAA_texture2DGetVOffset(scene, elem, &tex.v_offset);
    
    error = SAA_texture2DGetRepeats(scene, elem, &tex.u_repeat, &tex.v_repeat);
    
    error = SAA_texture2DGetCropMin(scene, elem, &tex.u_crop_min, &tex.v_crop_min);
    error = SAA_texture2DGetCropMax(scene, elem, &tex.u_crop_max, &tex.v_crop_max);
    
    error = SAA_texture2DGetAnim(scene, elem, &tex.anim_type);
    
    error = SAA_texture2DGetMapping(scene, elem, &tex.mapping);
    
    error = SAA_texture2DGetMapComponent(scene, elem, &tex.map_component);
    
    error = SAA_texture2DGetMasking(scene, elem, &tex.masking);
    
    SAA_Boolean interpolate = FALSE;
    error = SAA_texture2DGetInterpolate(scene, elem, &interpolate);
    tex.interpolate = interpolate != FALSE;
    
    SAA_Boolean displacement = FALSE;
    error = SAA_texture2DGetDisplacement(scene, elem, &displacement);
    tex.displacement = displacement != FALSE;
    
    SAA_Boolean uvswap = FALSE;
    error = SAA_texture2DGetUVSwap(scene, elem, &uvswap);
    tex.uv_swap = uvswap != FALSE;
    
    SAA_Boolean uvwrap = FALSE;
    error = SAA_texture2DGetUVWrapping(scene, elem, &uvwrap);
    tex.uv_wrap = uvwrap != FALSE;
    
    SAA_Boolean alth = FALSE;
    SAA_Boolean altv = FALSE;
    error = SAA_texture2DGetAlternate(scene, elem, &alth, &altv);
    tex.alth = alth != FALSE;
    tex.altv = altv != FALSE;
    
    // Success!
    return error;
};

SI_Error SI_GetTexture3d(SAA_Scene *scene, SAA_Elem *elem, SI_Texture3d &tex) {
    SI_Error error = SI_GetElement(scene, elem, tex);
    if (error != SI_SUCCESS) { return error; }
    
    error = SAA_texture3DGetColor0(scene, elem, &tex.color0.r, &tex.color0.g, &tex.color0.b, &tex.color0.a);
    error = SAA_texture3DGetColor1(scene, elem, &tex.color1.r, &tex.color1.g, &tex.color1.b, &tex.color1.a);
    error = SAA_texture3DGetColor2(scene, elem, &tex.color2.r, &tex.color2.g, &tex.color2.b, &tex.color2.a);
    error = SAA_texture3DGetColor3(scene, elem, &tex.color3.r, &tex.color3.g, &tex.color3.b, &tex.color3.a);
    error = SAA_texture3DGetColor4(scene, elem, &tex.color4.r, &tex.color4.g, &tex.color4.b, &tex.color4.a);
    
    error = SAA_texture3DGetTranslation(scene, elem, &tex.translation.x, &tex.translation.y, &tex.translation.z);
    error = SAA_texture3DGetRotation(scene, elem, &tex.rotation.x, &tex.rotation.y, &tex.rotation.z);
    error = SAA_texture3DGetScaling(scene, elem, &tex.scaling.x, &tex.scaling.y, &tex.scaling.z);
    
    error = SAA_texture3DGetFactor1(scene, elem, &tex.factor.x);
    error = SAA_texture3DGetFactor2(scene, elem, &tex.factor.y);
    error = SAA_texture3DGetFactor3(scene, elem, &tex.factor.z);
    
    error = SAA_texture3DGetAmbient(scene, elem, &tex.ambient);
    error = SAA_texture3DGetBlending(scene, elem, &tex.blending);
    error = SAA_texture3DGetDiffuse(scene, elem, &tex.diffuse);
    error = SAA_texture3DGetPower(scene, elem, &tex.power);
    error = SAA_texture3DGetReflection(scene, elem, &tex.reflection);
    error = SAA_texture3DGetRoughness(scene, elem, &tex.roughness);
    error = SAA_texture3DGetStrength(scene, elem, &tex.strength);
    error = SAA_texture3DGetSpacing(scene, elem, &tex.spacing);
    error = SAA_texture3DGetTapering(scene, elem, &tex.tapering);
    error = SAA_texture3DGetTransparency(scene, elem, &tex.transparency);
    error = SAA_texture3DGetIteration(scene, elem, &tex.iteration);
    error = SAA_texture3DGetMasking(scene, elem, &tex.masking);
    
    return error;
}

SI_Error SI_GetMaterial(SAA_Scene *scene, SAA_Elem *elem, SI_Material &mat) {
    SI_Error error = SI_GetElement(scene, elem, mat);
    if (error != SI_SUCCESS) { return error; }
    
    // Get the amount of active 2d textures for this material.
    const void *active_relinfo = nullptr;
    int num_active_tex2d = -1;
    error = SAA_materialRelationGetT2DLocNbElements(scene, elem, TRUE, &active_relinfo, &num_active_tex2d);
    mat.num_active_tex2d = num_active_tex2d;
    
    // Get the amount of passive 2d textures for this material.
    const void *passive_relinfo = nullptr;
    int num_passive_tex2d = -1;
    error = SAA_materialRelationGetT2DLocNbElements(scene, elem, FALSE, &passive_relinfo, &num_passive_tex2d);
    mat.num_passive_tex2d = num_passive_tex2d;
    
    // Get the amount of active 3d textures for this material.
    active_relinfo = nullptr;
    int num_active_tex3d = -1;
    error = SAA_materialRelationGetT3DLocNbElements(scene, elem, TRUE, &active_relinfo, &num_active_tex3d);
    mat.num_active_tex3d = num_active_tex3d;
    
    // Get the amount of passive 3d textures for this material.
    passive_relinfo = nullptr;
    int num_passive_tex3d = -1;
    error = SAA_materialRelationGetT3DLocNbElements(scene, elem, FALSE, &passive_relinfo, &num_passive_tex3d);
    mat.num_passive_tex3d = num_passive_tex3d;
    
    // Get the active 3d textures from this material.
    SAA_Elem *active_tex3d = new SAA_Elem[num_active_tex3d + 1];
    mat.active_tex3d = new SI_Texture3d[num_active_tex3d + 1]();
    error = SAA_materialRelationGetT3DLocElements(scene, elem, active_relinfo, num_active_tex3d, active_tex3d);
    for (uint32_t i = 0; i < num_active_tex3d; i++) {
        SI_Texture3d &tex = mat.active_tex3d[i];
        error = SI_GetTexture3d(scene, &active_tex3d[i], tex);
    }
    delete[] active_tex3d;
    
    // Get the passive 3d textures from this material.
    SAA_Elem *passive_tex3d = new SAA_Elem[num_passive_tex3d + 1];
    mat.passive_tex3d = new SI_Texture3d[num_passive_tex3d + 1]();
    error = SAA_materialRelationGetT3DLocElements(scene, elem, passive_relinfo, num_passive_tex3d, passive_tex3d);
    for (uint32_t i = 0; i < num_passive_tex3d; i++) {
        SI_Texture3d &tex = mat.passive_tex3d[i];
        error = SI_GetTexture3d(scene, &passive_tex3d[i], tex);
    }
    delete[] passive_tex3d;
    
    error = SAA_materialGetAmbient(scene, elem, &mat.ambieance.r, &mat.ambieance.g, &mat.ambieance.b);
    error = SAA_materialGetDiffuse(scene, elem, &mat.diffuse.r, &mat.diffuse.g, &mat.diffuse.b);
    error = SAA_materialGetSpecular(scene, elem, &mat.specular.r, &mat.specular.g, &mat.specular.b);
    error = SAA_materialGetSpecularDecay(scene, elem, &mat.specular_decay);
    error = SAA_materialGetReflection(scene, elem, &mat.reflection);
    error = SAA_materialGetRefractiveIndex(scene, elem, &mat.refractive_index);
    error = SAA_materialGetTransparency(scene, elem, &mat.transparency);
    error = SAA_materialGetBlurDecay(scene, elem, &mat.blur_decay);
    error = SAA_materialGetBlurWidth(scene, elem, &mat.blur_width);
    error = SAA_materialGetShadingModel(scene, elem, &mat.smodel);
    error = SAA_materialGetDiffuseSrc(scene, elem, &mat.diffuse_src);
    error = SAA_materialGetTransparencySrc(scene, elem, &mat.transparency_src);
    
    SAA_Boolean static_blur = FALSE;
    error = SAA_materialGetStaticBlur(scene, elem, &static_blur);
    mat.static_blur = static_blur == TRUE;
    
    return error;
}

SI_Error SI_GetPositionConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *relinfo = nullptr;
    int num_active_elem = 0;
    error = SAA_modelRelationGetCnsPosNbElements(scene, elem, 1, (const void **)&relinfo, &num_active_elem);
    
    relinfo = nullptr;
    int num_passive_elem = 0;
    error = SAA_modelRelationGetCnsPosNbElements(scene, elem, 0, (const void **)&relinfo, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Position Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    error = SAA_modelRelationGetCnsPosElements(scene, &cns.passive_elem, relinfo, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    error = SAA_modelRelationGetCnsPosActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = POSITION;

    return error;
}

SI_Error SI_GetOrientationConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsOriNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsOriNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Orientation Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    SAA_modelRelationGetCnsOriElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsOriActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = ORIENTATION;

    return SI_SUCCESS;
}

SI_Error SI_GetScaleConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsSclNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsSclNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Scale Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    SAA_modelRelationGetCnsSclElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsSclActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = SCALE;

    return SI_SUCCESS;
}

SI_Error SI_GetPositionLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_PositionLimitConstraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsPosLimNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsPosLimNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Position Limits Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    SAA_modelRelationGetCnsPosLimElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsPosLimActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = POSITION_LIMIT;
    
    // Get and store the minimum position.
    SAA_modelRelationGetCnsPosLimMin(scene, elem, elem, &cns.min_pos.x, &cns.min_pos.y, &cns.min_pos.z);

    // Get and store the maximum position.
    SAA_modelRelationGetCnsPosLimMax(scene, elem, elem, &cns.max_pos.x, &cns.max_pos.y, &cns.max_pos.z);
    
    // Get and store our damping attributes.
    SAA_modelRelationGetCnsPosLimDamping(scene, elem, elem, &cns.damping_width, &cns.damping_strength);
    
    // Get and store our radius.
    SAA_modelRelationGetCnsPosLimRadius(scene, elem, elem, &cns.radius);
    
    // Active booleans.
    SAA_Boolean x_active = FALSE;
    SAA_Boolean y_active = FALSE;
    SAA_Boolean z_active = FALSE;
    
    // Get and store the minimum position activeness.
    SAA_modelRelationGetCnsPosLimMinActive(scene, elem, elem, &x_active, &y_active, &z_active);
    cns.min_pos_active[0] = x_active != FALSE;
    cns.min_pos_active[1] = y_active != FALSE;
    cns.min_pos_active[2] = z_active != FALSE;

    // Get and store the maximum position activeness.
    SAA_modelRelationGetCnsPosLimMaxActive(scene, elem, elem, &x_active, &y_active, &z_active);
    cns.max_pos_active[0] = x_active != FALSE;
    cns.max_pos_active[1] = y_active != FALSE;
    cns.max_pos_active[2] = z_active != FALSE;
        
    // Get and store the limit type (shape).
    SAA_modelRelationGetCnsPosLimType(scene, elem, elem, &cns.shape);
    
    // Get and store the coordinate system.
    SAA_modelRelationGetCnsPosLimCoordSys(scene, elem, elem, &cns.sys);

    return SI_SUCCESS;
}

SI_Error SI_GetRotationLimitsConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_RotationLimitConstraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsRotLimNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsRotLimNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Rotation Limits Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    SAA_modelRelationGetCnsRotLimElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsRotLimActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = ROTATION_LIMIT;
    
    // Get and store the minimum position.
    SAA_modelRelationGetCnsRotLimMin(scene, elem, elem, &cns.min_hpr.x, &cns.min_hpr.y, &cns.min_hpr.z);

    // Get and store the maximum position.
    SAA_modelRelationGetCnsRotLimMax(scene, elem, elem, &cns.max_hpr.x, &cns.max_hpr.y, &cns.max_hpr.z);
    
    // Get and store our damping attributes.
    SAA_modelRelationGetCnsRotLimDamping(scene, elem, elem, &cns.damping_width, &cns.damping_strength);
    
    // Active booleans.
    SAA_Boolean h_active = FALSE;
    SAA_Boolean p_active = FALSE;
    SAA_Boolean r_active = FALSE;
    
    // Get and store the minimum position activeness.
    SAA_modelRelationGetCnsRotLimMinActive(scene, elem, elem, &h_active, &p_active, &r_active);
    cns.min_hpr_active[0] = h_active != FALSE;
    cns.min_hpr_active[1] = p_active != FALSE;
    cns.min_hpr_active[2] = r_active != FALSE;

    // Get and store the maximum position activeness.
    SAA_modelRelationGetCnsRotLimMaxActive(scene, elem, elem, &h_active, &p_active, &r_active);
    cns.max_hpr_active[0] = h_active != FALSE;
    cns.max_hpr_active[1] = p_active != FALSE;
    cns.max_hpr_active[2] = r_active != FALSE;

    return SI_SUCCESS;
}

SI_Error SI_GetUpVctConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsUpVctNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsUpVctNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
    if (num_passive_elem > 1) { printf("WARNING: More then one passive element dectected for Up Vct Constraint!\n"); }
    
    // Only save a constraint if we have active elements.
    if (num_active_elem <= 0) { return SI_ERR_WRONG_COUNT; }
    
    // Save our active element count.
    cns.num_active_elems = num_active_elem;
    
    // The passive element IS our argument element. No need to grab it again.
    memcpy(&cns.passive_elem, elem, sizeof(SAA_Elem));
    
    // Allocate buffer for all of the active SAA Elements.
    cns.active_elems = new SAA_Elem[cns.num_active_elems + 1]();
    // Extract all of the active elements into the buffer.
    SAA_modelRelationGetCnsUpVctElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsUpVctActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = UP_VCT;

    return SI_SUCCESS;
}

SI_Error SI_GetFCurve(SAA_Scene *scene, SAA_Elem *fcurve, SI_FCurve &curv) {
    SI_Error error = SI_GetElement(scene, fcurve, curv);
    if (error != SI_SUCCESS) { return error; }
    
    // Get the number of owners (1). Anything other then 1 is an error.
    int numowners = 0;
    error = SAA_fcurveGetNbOwners(scene, fcurve, &numowners);
    assert(numowners == 1);
    
    if (error != SI_ERR_NOTSUPPORTED) {
        // Get the owner. TODO: Assign ourself to our owner.
        SAA_Elem owner;
        error = SAA_fcurveGetOwners(scene, fcurve, numowners, &owner);
    } else {
        // Reset the error.
        error = SI_SUCCESS;
    }
    
    // Get the length of our the trackname.
    int tracknamelen = -1;
    error = SAA_fcurveGetTracknameLength(scene, fcurve, &tracknamelen);
    assert(tracknamelen != -1);
    curv.trackname_len = tracknamelen;
    
    // Get our trackname.
    char *trackname = new char[++tracknamelen]();
    error = SAA_fcurveGetTrackname(scene, fcurve, tracknamelen, trackname);
    curv.trackname = trackname;
    
    // Get if this fcurve is active or not.
    SAA_Boolean factive = 0;
    error = SAA_fcurveGetActive(scene, fcurve, &factive);
    curv.active = factive != FALSE;
    
    // Get interpolation mode for this fcurve.
    error = SAA_fcurveGetInterpolation(scene, fcurve, &curv.interp);
    
    // Get our extrapolation modes for this fcurve.
    error = SAA_fcurveGetPreExtrapolation(scene, fcurve, &curv.preextrap);
    error = SAA_fcurveGetPostExtrapolation(scene, fcurve, &curv.postextrap);
    
    // Get the amount of curve keys we have.
    int numkeys = -1;
    error = SAA_fcurveGetNbKeys(scene, fcurve, &numkeys);
    assert(numkeys != -1);
    curv.num_keys = numkeys;
    
    // Get all of our keys!
    SAA_SubElem *keys = new SAA_SubElem[curv.num_keys + 1];
    error = SAA_fcurveGetKeys(scene, fcurve, numkeys, keys);
    
    // Get all of the data for each key.
    SAA_FcurveInterpType *interps = new SAA_FcurveInterpType[curv.num_keys + 1];
    error = SAA_fcurveKeyGetInterpolation(scene, fcurve, numkeys, keys, interps);
    
    float *lslopes = new float[curv.num_keys + 1];
    float *rslopes = new float[curv.num_keys + 1];
    error = SAA_fcurveKeyGetSlope(scene, fcurve, numkeys, keys, lslopes, rslopes);
    
    SAA_FKeySlopeType *slopetypes = new SAA_FKeySlopeType[curv.num_keys + 1];
    error = SAA_fcurveKeyGetSlopeType(scene, fcurve, numkeys, keys, slopetypes);
    
    float *times = new float[curv.num_keys + 1];
    error = SAA_fcurveKeyGetTime(scene, fcurve, numkeys, keys, times);
    
    SAA_FKeyType *types = new SAA_FKeyType[curv.num_keys + 1];
    error = SAA_fcurveKeyGetType(scene, fcurve, numkeys, keys, types);
    
    float *values = new float[curv.num_keys + 1];
    error = SAA_fcurveKeyGetValue(scene, fcurve, numkeys, keys, values);
    
    // Iterate the key elements and apply the info for each key into our key structure.
    curv.keys = new SI_FCurveKey[curv.num_keys + 1]();
    for (uint32_t i = 0; i < curv.num_keys; i++) {
        SI_FCurveKey &key = curv.keys[i];
        key.value = values[i];
        key.time = times[i];
        key.lslope = lslopes[i];
        key.rslope = rslopes[i];
        key.slopetype = slopetypes[i];
        key.interp = interps[i];
        key.type = types[i];
    }
    
    // Free all of the allocated SI arrays.
    delete[] values;
    delete[] types;
    delete[] times;
    delete[] slopetypes;
    delete[] rslopes;
    delete[] lslopes;
    delete[] interps;
    delete[] keys;
    
    return error;
};

SI_Error SI_GetScene(SAA_Scene *scene, SI_Scene &siscene) {
    SI_Error error = SI_SUCCESS;
    
    // First let's make sure to optimize the scene in any way we can.
    
    // Optimize all of our 2d textures.
    int num_tex_processed = 0;
    int num_tex_merged = 0;
    int total_tex_mem = 0;
    error = SAA_sceneOptimizeTexture2D(scene, &num_tex_processed, &num_tex_merged, &total_tex_mem);
    assert(error == SI_SUCCESS);
    
    // Optimize all of our materials.
    int num_mats_merged = 0;
    error = SAA_sceneOptimizeMaterials(scene, &num_mats_merged);
    assert(error == SI_SUCCESS);
    
    // Now we should get the scene specfic data.
    
    // Get the length of our scenes prefix.
    int prefixlength = -1;
    error = SAA_sceneGetPrefixLength(scene, &prefixlength);
    assert(error == SI_SUCCESS);
    siscene.prefix_len = prefixlength;
    
    // Get the prefix of our scene.
    siscene.prefix = new char[++prefixlength]();
    error = SAA_sceneGetPrefix(scene, prefixlength, siscene.prefix);
    assert(error == SI_SUCCESS);
    
    // Get the length of our scenes name.
    int namelength = -1;
    error = SAA_sceneGetNameLength(scene, &namelength);
    assert(error == SI_SUCCESS);
    siscene.name_len = namelength;
    
    // Get the name of our scene.
    siscene.name = new char[++namelength]();
    error = SAA_sceneGetName(scene, namelength, siscene.name);
    assert(error == SI_SUCCESS);
    
    // Get the scaling type for the scene.
    error = SAA_sceneGetScalingType(scene, &siscene.scaling);
    assert(error == SI_SUCCESS);
    
    // Finally get all of the elements in the scene itself.
    
    // Get the number of fcurves.
    int num_fcurves = -1;
    error = SAA_sceneGetNbFcurves(scene, &num_fcurves);
    assert(error == SI_SUCCESS);
    siscene.num_fcurves = num_fcurves;
    
    // Get the fcurves in the scene.
    SAA_Elem *fcurves = new SAA_Elem[num_fcurves + 1];
    siscene.fcurves = new SI_FCurve[num_fcurves + 1]();
    error = SAA_sceneGetFcurves(scene, num_fcurves, fcurves);
    assert(error == SI_SUCCESS);
    for (uint32_t i = 0; i < num_fcurves; i++) {
        SAA_Elem *fcurve = &fcurves[i];
        error = SI_GetFCurve(scene, fcurve, siscene.fcurves[i]);
    }
    delete[] fcurves;
    
    // Get the number of 2d textures in the scene.
    int num_textures2d = -1;
    error = SAA_sceneGetNbTexture2D(scene, &num_textures2d);
    assert(error == SI_SUCCESS && num_textures2d != -1);
    siscene.num_textures2d = num_textures2d;
    
    // Get the 2d textures in the scene.
    SAA_Elem *textures2d = new SAA_Elem[num_textures2d + 1];
    siscene.textures2d = new SI_Texture2d[num_textures2d + 1]();
    error = SAA_sceneGetTexture2D(scene, num_textures2d, textures2d);
    assert(error == SI_SUCCESS);
    for (uint32_t i = 0; i < num_textures2d; i++) {
        SAA_Elem *texture2d = &textures2d[i];
        error = SI_GetTexture2d(scene, texture2d, siscene.textures2d[i]);
    }
    delete[] textures2d;
    
    // Get the number of 3d textures in the scene.
    int num_textures3d = -1;
    error = SAA_sceneGetNbTexture3D(scene, &num_textures3d);
    assert(error == SI_SUCCESS && num_textures3d != -1);
    siscene.num_textures3d = num_textures3d;
    
    // Get the 3d textures in the scene.
    SAA_Elem *textures3d = new SAA_Elem[num_textures3d + 1];
    siscene.textures3d = new SI_Texture3d[num_textures3d + 1]();
    error = SAA_sceneGetTexture3D(scene, num_textures3d, textures3d);
    assert(error == SI_SUCCESS);
    for (uint32_t i = 0; i < num_textures3d; i++) {
        SAA_Elem *texture3d = &textures3d[i];
        error = SI_GetTexture3d(scene, texture3d, siscene.textures3d[i]);
    }
    delete[] textures3d;
    
    // Get the number of materials in the scene.
    int num_materials = -1;
    error = SAA_sceneGetNbMaterials(scene, &num_materials);
    assert(error == SI_SUCCESS && num_materials != -1);
    siscene.num_materials = num_materials;
    
    // Get the materials in the scene.
    SAA_Elem *materials = new SAA_Elem[num_materials + 1];
    siscene.materials = new SI_Material[num_materials + 1]();
    error = SAA_sceneGetMaterials(scene, num_materials, materials);
    assert(error == SI_SUCCESS);
    for (uint32_t i = 0; i < num_materials; i++) {
        SAA_Elem *material = &materials[i];
        error = SI_GetMaterial(scene, material, siscene.materials[i]);
    }
    delete[] materials;
    
    return error;
}

SI_Error SI_WriteElement(SI_Element &elem, BinaryFile *file) {
    if (file == nullptr) { return SI_ERR_BAD_ARGUMENT; }
    
    file->write(elem.prefix_len);
    file->write(elem.prefix);
    file->write(elem.name_len);
    file->write(elem.name);
    file->write(elem.revision);
    file->write(elem.chapter);
    file->write(elem.wiretype);
    file->write(elem.wireframecol);
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_WriteTexture2d(SI_Texture2d &tex, BinaryFile *file) {
    SI_Error error = SI_WriteElement(tex, file);
    if (error != SI_SUCCESS) { return error; }
  
    // Write out the flags for our 2d texture.
    uint8_t flags = SI_Texture2dFlags::None;
    if (tex.interpolate) { flags = flags | SI_Texture2dFlags::Interpolate; }
    if (tex.displacement) { flags = flags | SI_Texture2dFlags::Displacement; }
    if (tex.uv_swap) { flags = flags | SI_Texture2dFlags::UVSwap; }
    if (tex.uv_wrap) { flags = flags | SI_Texture2dFlags::UVWrap; }
    if (tex.alth) { flags = flags | SI_Texture2dFlags::AltH; }
    if (tex.altv) { flags = flags | SI_Texture2dFlags::AltV; }

    file->write(flags);
    
    file->write(tex.filepath_len);
    file->write(tex.filepath);
    file->write(tex.transparency);
    file->write(tex.ambieance);
    file->write(tex.roughness);
    file->write(tex.rotation);
    file->write(tex.specular);
    file->write(tex.diffuse);
    file->write(tex.blend);
    file->write(tex.reflection);
    file->write(tex.reflection_int);
    file->write(tex.u_scale);
    file->write(tex.v_scale);
    file->write(tex.u_offset);
    file->write(tex.v_offset);
    file->write(tex.u_repeat);
    file->write(tex.v_repeat);
    file->write(tex.u_crop_min);
    file->write(tex.v_crop_min);
    file->write(tex.u_crop_max);
    file->write(tex.v_crop_max);
    file->write(tex.anim_type);
    file->write(tex.mapping);
    file->write(tex.map_component);
    file->write(tex.masking);
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_WriteTexture3d(SI_Texture3d &tex, BinaryFile *file) {
    SI_Error error = SI_WriteElement(tex, file);
    if (error != SI_SUCCESS) { return error; }
    
    file->write(tex.masking);
    file->write(tex.color0);
    file->write(tex.color1);
    file->write(tex.color2);
    file->write(tex.color3);
    file->write(tex.color4);
    file->write(tex.translation);
    file->write(tex.rotation);
    file->write(tex.scaling);
    file->write(tex.factor);
    file->write(tex.ambient);
    file->write(tex.blending);
    file->write(tex.diffuse);
    file->write(tex.power);
    file->write(tex.reflection);
    file->write(tex.roughness);
    file->write(tex.strength);
    file->write(tex.spacing);
    file->write(tex.tapering);
    file->write(tex.transparency);
    file->write(tex.iteration);
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_WriteFCurve(SI_FCurve &curv, BinaryFile *file) {
    SI_Error error = SI_WriteElement(curv, file);
    if (error != SI_SUCCESS) { return error; }
    
    file->write(curv.trackname_len);
    file->write(curv.trackname);
    file->write(curv.interp);
    file->write(curv.preextrap);
    file->write(curv.postextrap);
    file->write(curv.num_keys);
    
    for (uint32_t i = 0; i < curv.num_keys; i++) {
        SI_FCurveKey &key = curv.keys[i];
        file->write(key.type);
        file->write(key.slopetype);
        file->write(key.interp);
        file->write(key.value);
        file->write(key.time);
        file->write(key.lslope);
        file->write(key.rslope);
    }
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_WriteScene(SI_Scene &siscene, BinaryFile *file) {
    if (file == nullptr) { return SI_ERR_BAD_ARGUMENT; }
    
    file->write(siscene.prefix_len);
    file->write(siscene.prefix);
    file->write(siscene.name_len);
    file->write(siscene.name);
    file->write(siscene.num_elements);
    file->write(siscene.num_textures2d);
    file->write(siscene.num_textures3d);
    file->write(siscene.num_materials);
    file->write(siscene.num_fcurves);
    file->write(siscene.num_constraints);
    file->write(siscene.scaling);
    
    for (uint32_t i = 0; i < siscene.num_textures2d; i++) {
        SI_Texture2d &tex = siscene.textures2d[i];
        SI_WriteTexture2d(tex, file);
    }
    
    for (uint32_t i = 0; i < siscene.num_textures3d; i++) {
        SI_Texture3d &tex = siscene.textures3d[i];
        SI_WriteTexture3d(tex, file);
    }
    
    for (uint32_t i = 0; i < siscene.num_fcurves; i++) {
        SI_FCurve &fcurve = siscene.fcurves[i];
        SI_WriteFCurve(fcurve, file);
    }
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_CleanupElement(SI_Element &elem) {
    if (elem.prefix != nullptr) { delete[] elem.prefix; }
    if (elem.name != nullptr) { delete[] elem.name; }
    
    memset(&elem, 0, sizeof(SI_Element));
    
    return SI_SUCCESS;
}

SI_Error SI_CleanupTexture2d(SI_Texture2d &tex) {
    if (tex.prefix != nullptr) { delete[] tex.prefix; }
    if (tex.name != nullptr) { delete[] tex.name; }
    if (tex.filepath != nullptr) { delete[] tex.filepath; }
    
    memset(&tex, 0, sizeof(SI_Texture2d));
    
    return SI_SUCCESS;
}