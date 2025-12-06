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
    int prefixlength = 0;
    SAA_elementGetPrefixLength(scene, elem, &prefixlength);
    
    // Get the prefix of our element.
    si_elem.prefix = new char[prefixlength + 1]();
    SAA_elementGetPrefix(scene, elem, prefixlength, si_elem.prefix);
    
    // Get the length of our elements name.
    int namelength = 0;
    SAA_elementGetNameLength(scene, elem, &namelength);
    
    // Get the name of our element.
    si_elem.name = new char[namelength + 1]();
    SAA_elementGetName(scene, elem, namelength, si_elem.name);

    // Get the reivision of the element.
    SAA_elementGetRevision(scene, elem, &si_elem.revision);

    // Get the chapter for the Element. 
    // This is good for sorting and resolving the elements type later on if we need too.
    SAA_elementGetChapter(scene, elem, &si_elem.chapter);

    // Get the wireframe info.
    SAA_elementGetWireColor(scene, elem, &si_elem.wireframecol);
    SAA_elementGetWireType(scene, elem, &si_elem.wiretype);
    
    // Success!
    return SI_SUCCESS;
}

SI_Error SI_GetPositionConstraint(SAA_Scene *scene, SAA_Elem *elem, SI_Constraint &cns) {
    SI_Error error = SI_GetElement(scene, elem, cns);
    if (error != SI_SUCCESS) { return error; }
    
    void *rel_info = nullptr;
    
    int num_active_elem = 0;
    SAA_modelRelationGetCnsPosNbElements(scene, elem, 1, (const void **)&rel_info, &num_active_elem);
    
    int num_passive_elem = 0;
    SAA_modelRelationGetCnsPosNbElements(scene, elem, 0, (const void **)&rel_info, &num_passive_elem);
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
    SAA_modelRelationGetCnsPosElements(scene, &cns.passive_elem, rel_info, cns.num_active_elems, cns.active_elems);
    
    // Get if the constraint is active or inactive.
    SAA_Boolean active = FALSE;
    SAA_modelRelationGetCnsPosActive(scene, &cns.passive_elem, &cns.passive_elem, &active);
    // Set the activity of the constraint.
    cns.active = active == TRUE;
    
    // Set the constraint type.
    cns.type = POSITION;

    return SI_SUCCESS;
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
    SAA_fcurveGetNbOwners(scene, fcurve, &numowners);
    assert(numowners == 1);
    
    // Get the owner. TODO: Assign ourself to our owner.
    SAA_Elem owner;
    SAA_fcurveGetOwners(scene, fcurve, numowners, &owner);
    
    // Get the length of our the trackname.
    int tracknamelen = -1;
    SAA_fcurveGetTracknameLength(scene, fcurve, &tracknamelen);
    assert(tracknamelen != -1);
    
    // Get our trackname.
    char *trackname = new char[++tracknamelen]();
    SAA_fcurveGetTrackname(scene, fcurve, tracknamelen, trackname);
    curv.trackname = trackname;
    
    // Get if this fcurve is active or not.
    SAA_Boolean factive = 0;
    SAA_fcurveGetActive(scene, fcurve, &factive);
    curv.active = factive != FALSE;
    
    // Get interpolation mode for this fcurve.
    SAA_fcurveGetInterpolation(scene, fcurve, &curv.interp);
    
    // Get our extrapolation modes for this fcurve.
    SAA_fcurveGetPreExtrapolation(scene, fcurve, &curv.preextrap);
    SAA_fcurveGetPostExtrapolation(scene, fcurve, &curv.postextrap);
    
    // Get the amount of curve keys we have.
    int numkeys = -1;
    SAA_fcurveGetNbKeys(scene, fcurve, &numkeys);
    assert(numkeys != -1);
    curv.num_keys = numkeys;
    
    // Get all of our keys!
    SAA_SubElem *keys = new SAA_SubElem[curv.num_keys + 1];
    SAA_fcurveGetKeys(scene, fcurve, numkeys, keys);
    
    // Get all of the data for each key.
    SAA_FcurveInterpType *interps = new SAA_FcurveInterpType[curv.num_keys + 1];
    SAA_fcurveKeyGetInterpolation(scene, fcurve, numkeys, keys, interps);
    
    float *lslopes = new float[curv.num_keys + 1];
    float *rslopes = new float[curv.num_keys + 1];
    SAA_fcurveKeyGetSlope(scene, fcurve, numkeys, keys, lslopes, rslopes);
    
    SAA_FKeySlopeType *slopetypes = new SAA_FKeySlopeType[curv.num_keys + 1];
    SAA_fcurveKeyGetSlopeType(scene, fcurve, numkeys, keys, slopetypes);
    
    float *times = new float[curv.num_keys + 1];
    SAA_fcurveKeyGetTime(scene, fcurve, numkeys, keys, times);
    
    SAA_FKeyType *types = new SAA_FKeyType[curv.num_keys + 1];
    SAA_fcurveKeyGetType(scene, fcurve, numkeys, keys, types);
    
    float *values = new float[curv.num_keys + 1];
    SAA_fcurveKeyGetValue(scene, fcurve, numkeys, keys, values);
    
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
    
    // Success!
    return SI_SUCCESS;
};