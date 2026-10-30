/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_AUDIO_ELEMENT_H
#define LEXBOR_HTML_AUDIO_ELEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/interface.h"
#include "lexbor/html/interfaces/media_element.h"


struct lxb_html_audio_element {
    lxb_html_media_element_t media_element;
};


LXB_API lxb_html_audio_element_t *
lxb_html_audio_element_interface_create(lxb_html_document_t *document);

LXB_API lxb_html_audio_element_t *
lxb_html_audio_element_interface_destroy(lxb_html_audio_element_t *audio_element);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_AUDIO_ELEMENT_H */
