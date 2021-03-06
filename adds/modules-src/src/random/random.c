/*
 *            Copyright (C) 2013, 2014, 2015, 2016 by Rafael Santiago
 *
 * This is free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../../types.h"
#include <stdlib.h>

void hsl_random(struct hefesto_modio **modio) {
    if ((*modio) == NULL || (*modio)->args == NULL ||
        (*modio)->args->data == NULL) {
        return;
    }
    (*modio)->rtype = HEFESTO_VAR_TYPE_INT;
    if ((*modio)->args->dtype != HEFESTO_VAR_TYPE_INT) {
        return;
    }
    (*modio)->ret = (int *) malloc(sizeof(int));
    *(int *)(*modio)->ret = (rand() %
                              (*((int *)(*modio)->args->data)));
}

void hsl_randomseed(struct hefesto_modio **modio) {
    if ((*modio) == NULL || (*modio)->args == NULL ||
        (*modio)->args->data == NULL) {
        return;
    }
    (*modio)->rtype = HEFESTO_VAR_TYPE_NONE;
    (*modio)->ret = NULL;
    srand(*(int *)(*modio)->args->data);
}
