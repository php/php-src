/*
 * Copyright (C) 2018-2025 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/exception.h"
#include "lexbor/dom/interfaces/document.h"


typedef struct {
    lexbor_str_t name;
    lexbor_str_t message;
}
lxb_dom_exception_data_t;


static const lxb_dom_exception_data_t lxb_dom_exception_data[LXB_DOM_EXCEPTION__LAST_ENTRY] =
{
    {
        lexbor_str("Error"),
        lexbor_str("")
    },
    {
        lexbor_str("IndexSizeError"),
        lexbor_str("Deprecated. Use RangeError instead.")
    },
    {
        lexbor_str("DOMStringSizeError"),
        lexbor_str("")
    },
    {
        lexbor_str("HierarchyRequestError"),
        lexbor_str("The operation would yield an incorrect node tree.")
    },
    {
        lexbor_str("WrongDocumentError"),
        lexbor_str("The object is in the wrong document.")
    },
    {
        lexbor_str("InvalidCharacterError"),
        lexbor_str("The string contains invalid characters.")
    },
    {
        lexbor_str("NoDataAllowedError"),
        lexbor_str("")
    },
    {
        lexbor_str("NoModificationAllowedError"),
        lexbor_str("The object can not be modified.")
    },
    {
        lexbor_str("NotFoundError"),
        lexbor_str("The object can not be found here.")
    },
    {
        lexbor_str("NotSupportedError"),
        lexbor_str("The operation is not supported.")
    },
    {
        lexbor_str("InUseAttributeError"),
        lexbor_str("The attribute is in use by another element.")
    },
    {
        lexbor_str("InvalidStateError"),
        lexbor_str("The object is in an invalid state.")
    },
    {
        lexbor_str("SyntaxError"),
        lexbor_str("The string did not match the expected pattern.")
    },
    {
        lexbor_str("InvalidModificationError"),
        lexbor_str("The object can not be modified in this way.")
    },
    {
        lexbor_str("NamespaceError"),
        lexbor_str("The operation is not allowed by Namespaces in XML.")
    },
    {
        lexbor_str("InvalidAccessError"),
        lexbor_str("Deprecated. Use TypeError for invalid arguments, "
                   "\"NotSupportedError\" DOMException for unsupported operations, "
                   "and \"NotAllowedError\" DOMException for denied requests instead.")
    },
    {
        lexbor_str("ValidationError"),
        lexbor_str("")
    },
    {
        lexbor_str("TypeMismatchError"),
        lexbor_str("Deprecated. Use TypeError instead.")
    },
    {
        lexbor_str("SecurityError"),
        lexbor_str("The operation is insecure.")
    },
    {
        lexbor_str("NetworkError"),
        lexbor_str("A network error occurred.")
    },
    {
        lexbor_str("AbortError"),
        lexbor_str("The operation was aborted.")
    },
    {
        lexbor_str("URLMismatchError"),
        lexbor_str("Deprecated.")
    },
    {
        lexbor_str("QuotaExceededError"),
        lexbor_str("Deprecated. Use the QuotaExceededError DOMException-derived "
                   "interface instead.")
    },
    {
        lexbor_str("TimeoutError"),
        lexbor_str("The operation timed out.")
    },
    {
        lexbor_str("InvalidNodeTypeError"),
        lexbor_str("The supplied node is incorrect or has an incorrect ancestor "
                   "for this operation.")
    },
    {
        lexbor_str("DataCloneError"),
        lexbor_str("The object can not be cloned.")
    },
    {
        lexbor_str("EncodingError"),
        lexbor_str("The encoding operation (either encoded or decoding) failed.")
    },
    {
        lexbor_str("NotReadableError"),
        lexbor_str("The I/O read operation failed.")
    },
    {
        lexbor_str("UnknownError"),
        lexbor_str("The operation failed for an unknown transient reason "
                   "(e.g. out of memory).")
    },
    {
        lexbor_str("ConstraintError"),
        lexbor_str("A mutation operation in a transaction failed because a "
                   "constraint was not satisfied.")
    },
    {
        lexbor_str("DataError"),
        lexbor_str("Provided data is inadequate.")
    },
    {
        lexbor_str("TransactionInactiveError"),
        lexbor_str("A request was placed against a transaction which is currently"
                   " not active, or which is finished.")
    },
    {
        lexbor_str("ReadOnlyError"),
        lexbor_str("The mutating operation was attempted in a \"readonly\" transaction.")
    },
    {
        lexbor_str("VersionError"),
        lexbor_str("An attempt was made to open a database using a lower version"
                   " than the existing version.")
    },
    {
        lexbor_str("OperationError"),
        lexbor_str("The operation failed for an operation-specific reason.")
    },
    {
        lexbor_str("NotAllowedError"),
        lexbor_str("The request is not allowed by the user agent or the platform"
                   " in the current context, possibly because the user denied permission.")
    },
    {
        lexbor_str("OptOutError"),
        lexbor_str("The user opted out of the process.")
    }
};


lxb_dom_exception_t *
lxb_dom_exception_create(lxb_dom_document_t *document,
                         const lxb_char_t *message, size_t message_length,
                         const lxb_char_t *name, size_t name_length)
{
    lexbor_str_t *str;
    const lxb_dom_exception_data_t *data;
    lxb_dom_exception_t *exception;

    exception = lexbor_mraw_alloc(document->mraw, sizeof(lxb_dom_exception_t));
    if (exception == NULL) {
        return NULL;
    }

    exception->document = document;

    if (name != NULL && name_length > 0) {
        exception->code = lxb_dom_exception_code_by_name(name, name_length);
    }
    else {
        exception->code = LXB_DOM_EXCEPTION_ERR;
    }

    /* Message. */

    if (message == NULL || message_length == 0) {
        data = &lxb_dom_exception_data[exception->code];
        exception->message = *((lexbor_str_t *) &data->message);
    }
    else {
        str = &exception->message;
        str->data = lexbor_mraw_alloc(document->mraw, message_length + 1);
        if (str->data == NULL) {
            goto failed;
        }

        memcpy(str->data, message, message_length);

        str->data[message_length] = '\0';
        str->length = message_length;
    }

    /* Name. */

    if (exception->code != LXB_DOM_EXCEPTION_ERR
        || name == NULL || name_length == 0)
    {
        data = &lxb_dom_exception_data[exception->code];
        exception->name = *((lexbor_str_t *) &data->name);
    }
    else {
        str = &exception->name;
        str->data = lexbor_mraw_alloc(document->mraw, name_length + 1);
        if (str->data == NULL) {
            if (exception->message.length != 0) {
                lexbor_mraw_free(document->mraw, exception->message.data);
            }

            goto failed;
        }

        memcpy(str->data, name, name_length);

        str->data[name_length] = '\0';
        str->length = name_length;
    }

    return exception;

failed:

    lexbor_mraw_free(document->mraw, exception);

    return NULL;
}

lxb_dom_exception_t *
lxb_dom_exception_create_by_code(lxb_dom_document_t *document,
                                 const lxb_char_t *message, size_t length,
                                 lxb_dom_exception_code_t code)
{
    lexbor_str_t *str;
    const lxb_dom_exception_data_t *data;
    lxb_dom_exception_t *exception;

    if (code <= LXB_DOM_EXCEPTION_OK || code >= LXB_DOM_EXCEPTION__LAST_ENTRY) {
        return NULL;
    }

    exception = lexbor_mraw_alloc(document->mraw, sizeof(lxb_dom_exception_t));
    if (exception == NULL) {
        return NULL;
    }

    exception->document = document;
    exception->code = code;

    data = &lxb_dom_exception_data[code];

    /* Message. */

    if (message == NULL || length == 0) {
        exception->message = *((lexbor_str_t *) &data->message);
    }
    else {
        str = &exception->message;
        str->data = lexbor_mraw_alloc(document->mraw, length + 1);
        if (str->data == NULL) {
            goto failed;
        }

        memcpy(str->data, message, length);

        str->data[length] = '\0';
        str->length = length;
    }

    /* Name. */

    exception->name = *((lexbor_str_t *) &data->name);

    return exception;

failed:

    lexbor_mraw_free(document->mraw, exception);

    return NULL;
}

lxb_dom_exception_t *
lxb_dom_exception_destroy(lxb_dom_exception_t *exception)
{
    const lxb_dom_exception_data_t *data;
    lxb_dom_document_t *document = exception->document;

    data = &lxb_dom_exception_data[exception->code];

    if (exception->message.data != NULL
        && exception->message.data != data->message.data)
    {
        lexbor_mraw_free(document->mraw, exception->message.data);
    }

    if (exception->name.data != NULL
        && exception->name.data != data->name.data)
    {
        lexbor_mraw_free(document->mraw, exception->name.data);
    }

    lexbor_mraw_free(document->mraw, exception);

    return NULL;
}

const lexbor_str_t *
lxb_dom_exception_message_by_code(lxb_dom_exception_code_t code)
{
    if (code <= LXB_DOM_EXCEPTION_OK || code >= LXB_DOM_EXCEPTION__LAST_ENTRY) {
        return NULL;
    }

    return &lxb_dom_exception_data[code].message;
}

const lexbor_str_t *
lxb_dom_exception_name_by_code(lxb_dom_exception_code_t code)
{
    if (code <= LXB_DOM_EXCEPTION_OK || code >= LXB_DOM_EXCEPTION__LAST_ENTRY) {
        return NULL;
    }

    return &lxb_dom_exception_data[code].name;
}

lxb_dom_exception_code_t
lxb_dom_exception_code_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_str_t *str;
    lxb_dom_exception_code_t code;

    for (code = 0; code < LXB_DOM_EXCEPTION__LAST_ENTRY; code++) {
        str = &lxb_dom_exception_data[code].name;

        if (length == str->length
            && lexbor_str_data_ncasecmp(str->data, name, length))
        {
            return code;
        }
    }

    return LXB_DOM_EXCEPTION_ERR;
}
