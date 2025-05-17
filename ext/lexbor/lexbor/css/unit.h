/*
 * Copyright (C) 2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LXB_CSS_UNIT_H
#define LXB_CSS_UNIT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/css/base.h"


LXB_API const lxb_css_data_t *
lxb_css_unit_absolute_relative_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_absolute_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_relative_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_angel_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_frequency_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_resolution_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_duration_by_name(const lxb_char_t *name, size_t length);

LXB_API const lxb_css_data_t *
lxb_css_unit_by_id(lxb_css_type_t id);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LXB_CSS_UNIT_H */
