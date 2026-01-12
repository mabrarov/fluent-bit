/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2026 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_ES_TYPE_H
#define FLB_ES_TYPE_H

#include <fluent-bit/flb_mem.h>
#include <fluent-bit/flb_slist.h>
#include <fluent-bit/flb_str.h>
#include <fluent-bit/flb_sds.h>
#include <fluent-bit/flb_aws_credentials.h>
#include <fluent-bit/flb_record_accessor.h>
#include <fluent-bit/tls/flb_tls.h>

/**
 * C-string with ownership flag.
 *
 * It is not allowed to write members directly, functions like flb_es_str_*
 * should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_str {
    /* The actual C-string data. */
    char *value;
    /**
     * A flag indicating if this struct owns the C-string.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};

/**
 * Simple Dynamic String with ownership flag.
 *
 * It is not allowed to write members directly, functions like flb_es_sds_*
 * should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_sds_t {
    /* The actual Simple Dynamic String data. */
    flb_sds_t value;
    /**
     * A flag indicating if this struct owns the string.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};

/**
 * List with ownership flag.
 *
 * It is not allowed to write members directly, functions like flb_es_list_*
 * should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_list {
    /* The actual list data. */
    struct mk_list *value;
    /**
     * A flag indicating if this struct owns the list.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};

/**
 * TLS context with ownership flag.
 *
 * It is not allowed to write members directly, functions like flb_es_tls_*
 * should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_tls {
    /* The actual TLS context data. */
    struct flb_tls *value;
    /**
     * A flag indicating if this struct owns the TLS context.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};

/**
 * Record accessor with ownership flag.
 *
 * It is not allowed to write members directly, functions like flb_es_ra_*
 * should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_record_accessor {
    /* The actual record accessor data. */
    struct flb_record_accessor *value;
    /**
     * A flag indicating if this struct owns the record accessor.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};

#ifdef FLB_HAVE_AWS
/**
 * AWS provider with ownership flag.
 *
 * It is not allowed to write members directly, functions like
 * flb_es_aws_provider_* should be used instead. The only exceptions are:
 * - initialization of struct,
 * - modification of @c value when @c owned is FLB_FALSE.
 */
struct flb_es_aws_provider {
    /* The actual AWS provider data. */
    struct flb_aws_provider *value;
    /**
     * A flag indicating if this struct owns the AWS provider.
     * FLB_FALSE implies no ownership.
     */
    int owned;
};
#endif

/**
 * If C-string represented by @c value of ins is owned by ins,
 * then frees @c value of ins using flb_free.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_str, NULL is not allowed.
 */
static inline void flb_es_str_destroy(struct flb_es_str *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_free(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given C-string src with no ownership implied.
 *
 * @param dest The target instance of flb_es_str, NULL is not allowed.
 * @param src  The source C-string, NULL is allowed.
 *             It is not allowed for C-string represented by src to intersect
 *             with C-string represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_str_set_str(struct flb_es_str *dest, char *src)
{
    flb_es_str_destroy(dest);
    dest->value = src;
    dest->owned = FLB_FALSE;
}

/**
 * Duplicates C-string src using flb_strdup, then if succeeds destroys existing
 * @c value of dest (if it is owned by dest) and assigns @c value of dest
 * result of flb_strdup implying this C-string is owned by dest.
 *
 * @param dest The target instance of flb_es_str, NULL is not allowed.
 * @param src  The source C-string, NULL is allowed. If src is NULL,
 *             then flb_strdup is not called, @c value of dest
 *             is assigned NULL and NULL is returned.
 *             It is not allowed for C-string represented by src to intersect
 *             with C-string represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 *
 * @return The value assigned to @c value of dest if src is not NULL and
 *         flb_strdup succeeds, NULL otherwise.
 */
static inline char* flb_es_str_copy_str(struct flb_es_str *dest,
                                        const char *src)
{
    char *dup;
    if (!src) {
        flb_es_str_destroy(dest);
        return NULL;
    }
    dup = flb_strdup(src);
    if (!dup) {
        return NULL;
    }
    flb_es_str_destroy(dest);
    dest->value = dup;
    dest->owned = FLB_TRUE;
    return dup;
}

/**
 * If Simple Dynamic String represented by @c value of ins is owned by ins,
 * then destroys @c value of ins using flb_sds_destroy.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_sds_t, NULL is not allowed.
 */
static inline void flb_es_sds_destroy(struct flb_es_sds_t *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_sds_destroy(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * Destroys @c value of dest if @c value is owned,
 * then assigns @c value of dest given Simple Dynamic String src
 * with no ownership implied.
 *
 * @param dest The target instance of flb_es_sds_t, NULL is not allowed.
 * @param src  The source Simple Dynamic String, NULL is allowed.
 *             It is not allowed for string represented by src to intersect
 *             with string represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_sds_set_sds(struct flb_es_sds_t *dest, flb_sds_t src)
{
    flb_es_sds_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_FALSE;
    }
}

/**
 * Duplicates C-string src using flb_sds_create, then if flb_sds_create succeeds
 * destroys @c value of dest (if it is owned by dest) and
 * assigns @c value of dest result of flb_sds_create implying this SDS
 * is owned by dest.
 *
 * @param dest The target instance of flb_es_sds_t, NULL is not allowed.
 * @param src  The source C-string, NULL is allowed. If src is NULL,
 *             then flb_sds_create is not called, @c value of dest is assigned
 *             NULL and NULL is returned.
 *             It is not allowed for C-string represented by src to intersect
 *             with string represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 *
 * @return The value assigned to @c value of dest if src is not NULL and
 *         flb_sds_create succeeds, NULL otherwise.
 */
static inline char* flb_es_sds_copy_str(struct flb_es_sds_t *dest,
                                        const char *src)
{
    flb_sds_t dup;
    if (!src) {
        flb_es_sds_destroy(dest);
        return NULL;
    }
    dup = flb_sds_create(src);
    if (!dup) {
        return NULL;
    }
    flb_es_sds_destroy(dest);
    dest->value = dup;
    dest->owned = FLB_TRUE;
    return dup;
}

/**
 * If list represented by @c value of ins is owned by ins,
 * then destroys list using flb_slist_destroy and frees memory using flb_free.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_list, NULL is not allowed.
 */
static inline void flb_es_list_destroy(struct flb_es_list *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_slist_destroy(ins->value);
        flb_free(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given src with no ownership implied.
 *
 * @param dest The target instance of flb_es_list, NULL is not allowed.
 * @param src  The source list, NULL is allowed.
 *             It is not allowed for list represented by src to intersect
 *             with list represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_list_set_list(struct flb_es_list *dest,
                                        struct mk_list *src)
{
    flb_es_list_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_FALSE;
    }
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given src implying this list is owned by dest.
 *
 * @param dest The target instance of flb_es_list, NULL is not allowed.
 * @param src  The source list, NULL is allowed.
 *             It is not allowed for list represented by src to intersect
 *             with list represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_list_move_list(struct flb_es_list *dest,
                                         struct mk_list *src)
{
    flb_es_list_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_TRUE;
    }
}

/**
 * If TLS context represented by @c value of ins is owned by ins,
 * then destroys TLS context using flb_tls_destroy.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_tls, NULL is not allowed.
 */
static inline void flb_es_tls_destroy(struct flb_es_tls *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_tls_destroy(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given TLS context src
 * with no ownership implied.
 *
 * @param dest The target instance of flb_es_tls, NULL is not allowed.
 * @param src  The source TLS context, NULL is allowed.
 *             It is not allowed for TLS context represented by src to intersect
 *             with TLS context represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_tls_set_tls(struct flb_es_tls *dest,
                                      struct flb_tls *src)
{
    flb_es_tls_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_FALSE;
    }
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given TLS context src implying this TLS context
 * is owned by dest.
 *
 * @param dest The target instance of flb_es_tls, NULL is not allowed.
 * @param src  The source TLS context, NULL is allowed.
 *             It is not allowed for TLS context represented by src to intersect
 *             with TLS context represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_tls_move_tls(struct flb_es_tls *dest,
                                       struct flb_tls *src)
{
    flb_es_tls_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_TRUE;
    }
}

/**
 * If record accessor represented by @c value of ins is owned by ins,
 * then destroys record accessor using flb_ra_destroy.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_record_accessor,
 *            NULL is not allowed.
 */
static inline void flb_es_ra_destroy(struct flb_es_record_accessor *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_ra_destroy(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given record accessor src
 * with no ownership implied.
 *
 * @param dest The target instance of flb_es_record_accessor,
 *             NULL is not allowed.
 * @param src  The source record accessor, NULL is allowed.
 *             It is not allowed for record accessor represented by src
 *             to intersect with record accessor represented
 *             by @c value of dest, e.g. it is not allowed for src to be equal
 *             to @c value of dest.
 */
static inline void flb_es_ra_set_ra(struct flb_es_record_accessor *dest,
                                    struct flb_record_accessor *src)
{
    flb_es_ra_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_FALSE;
    }
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given record accessor src
 * implying this record accessor is owned by dest.
 *
 * @param dest The target instance of flb_es_record_accessor,
 *             NULL is not allowed.
 * @param src  The source record accessor, NULL is allowed.
 *             It is not allowed for record accessor represented by src
 *             to intersect with record accessor represented
 *             by @c value of dest, e.g. it is not allowed for src to be equal
 *             to @c value of dest.
 */
static inline void flb_es_ra_move_ra(struct flb_es_record_accessor *dest,
                                     struct flb_record_accessor *src)
{
    flb_es_ra_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_TRUE;
    }
}

#ifdef FLB_HAVE_AWS

/**
 * If AWS provider represented by @c value of ins is owned by ins,
 * then destroys AWS provider using flb_aws_provider_destroy.
 *
 * Sets @c value of ins to NULL and @c owned of ins to FLB_FALSE.
 *
 * @param ins The destroyed instance of flb_es_aws_provider,
 *            NULL is not allowed.
 */
static inline void flb_es_aws_provider_destroy(struct flb_es_aws_provider *ins)
{
    if (ins->value && ins->owned != FLB_FALSE) {
        flb_aws_provider_destroy(ins->value);
    }
    ins->value = NULL;
    ins->owned = FLB_FALSE;
}

/**
 * If dest != src, then destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given AWS provider represented
 * by @c value of src with no ownership implied.
 *
 * If dest == src then does nothing.
 *
 * @param dest The target instance of flb_es_aws_provider, NULL is not allowed.
 * @param src  The source instance of flb_es_aws_provider, NULL is allowed.
 *             It is not allowed for AWS provider represented by @c value of src
 *             to intersect with AWS provider represented by @c value of dest
 *             (e.g. it is not allowed for @c value of src to be equal to
 *             @c value of dest), except when dest == src.
 */
static inline void flb_es_aws_provider_set(
    struct flb_es_aws_provider *dest, const struct flb_es_aws_provider *src)
{
    if (dest == src) {
        return;
    }
    flb_es_aws_provider_destroy(dest);
    if (src) {
        dest->value = src->value;
        dest->owned = FLB_FALSE;
    }
}

/**
 * If dest != src, then destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest @c value of src,
 * then @c value of src is assigned NULL.
 * If assigned value was owned by src, then it is implied to be owned by dest,
 * otherwise dest implies no ownership of its new value, i.e. both value and
 * ownership are transferred from src to dest.
 *
 * If dest == src then does nothing.
 *
 * @param dest The target instance of flb_es_aws_provider, NULL is not allowed.
 * @param src  The source instance of flb_es_aws_provider, NULL is allowed.
 *             It is not allowed for AWS provider represented by @c value of src
 *             to intersect with AWS provider represented by @c value of dest
 *             (e.g. it is not allowed for @c value of src to be equal to
 *             @c value of dest), except when dest == src.
 */
static inline void flb_es_aws_provider_move(struct flb_es_aws_provider *dest,
                                            struct flb_es_aws_provider *src)
{
    if (dest == src) {
        return;
    }
    flb_es_aws_provider_destroy(dest);
    if (src) {
        *dest = *src;
        src->value = NULL;
    }
}

/**
 * Destroys @c value of dest if @c value is owned by dest,
 * then assigns @c value of dest given AWS provider src implying this provider
 * is owned by dest.
 *
 * @param dest The target instance of flb_es_aws_provider, NULL is not allowed.
 * @param src  The source instance of flb_aws_provider, NULL is allowed.
 *             It is not allowed for AWS provider represented by src
 *             to intersect with AWS provider represented by @c value of dest,
 *             e.g. it is not allowed for src to be equal to @c value of dest.
 */
static inline void flb_es_aws_provider_move_provider(
    struct flb_es_aws_provider *dest, struct flb_aws_provider *src)
{
    flb_es_aws_provider_destroy(dest);
    if (src) {
        dest->value = src;
        dest->owned = FLB_TRUE;
    }
}

#endif

#endif
