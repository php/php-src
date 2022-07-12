<?php

/** @generate-class-entries */

/**
 * @var int
 * @cname LIBXML_VERSION
 */
const LIBXML_VERSION = UNKNOWN;
/**
 * @var string
 * @cname LIBXML_DOTTED_VERSION
 */
const LIBXML_DOTTED_VERSION = UNKNOWN;
/**
 * @var string
 * @cname PHP_LIBXML_LOADED_VERSION
 */
const LIBXML_LOADED_VERSION = UNKNOWN;

/**
 * @var int
 * @cname XML_PARSE_NOENT
 */
const LIBXML_NOENT = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_DTDLOAD
 */
const LIBXML_DTDLOAD = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_DTDATTR
 */
const LIBXML_DTDATTR = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_DTDVALID
 */
const LIBXML_DTDVALID = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NOERROR
 */
const LIBXML_NOERROR = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NOWARNING
 */
const LIBXML_NOWARNING = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NOBLANKS
 */
const LIBXML_NOBLANKS = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_XINCLUDE
 */
const LIBXML_XINCLUDE = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NSCLEAN
 */
const LIBXML_NSCLEAN = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NOCDATA
 */
const LIBXML_NOCDATA = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_NONET
 */
const LIBXML_NONET = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_PEDANTIC
 */
const LIBXML_PEDANTIC = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_COMPACT
 */
const LIBXML_COMPACT = UNKNOWN;
/**
 * @var int
 * @cname XML_SAVE_NO_DECL
 */
const LIBXML_NOXMLDECL = UNKNOWN;
/**
 * @var int
 * @cname XML_PARSE_HUGE
 */
const LIBXML_PARSEHUGE = UNKNOWN;
#if LIBXML_VERSION >= 20900
/**
 * @var int
 * @cname XML_PARSE_BIG_LINES
 */
const LIBXML_BIGLINES = UNKNOWN;
#endif
/**
 * @var int
 * @cname LIBXML_SAVE_NOEMPTYTAG
 */
const LIBXML_NOEMPTYTAG = UNKNOWN;

#ifdef LIBXML_SCHEMAS_ENABLED
/**
 * @var int
 * @cname XML_SCHEMA_VAL_VC_I_CREATE
 */
const LIBXML_SCHEMA_CREATE = UNKNOWN;
#endif

#if LIBXML_VERSION >= 20707
/**
 * @var int
 * @cname HTML_PARSE_NOIMPLIED
 */
const LIBXML_HTML_NOIMPLIED = UNKNOWN;
#endif

#if LIBXML_VERSION >= 20708
/**
 * @var int
 * @cname HTML_PARSE_NODEFDTD
 */
const LIBXML_HTML_NODEFDTD = UNKNOWN;
#endif

/**
 * @var int
 * @cname XML_ERR_NONE
 */
const LIBXML_ERR_NONE = UNKNOWN;
/**
 * @var int
 * @cname XML_ERR_WARNING
 */
const LIBXML_ERR_WARNING = UNKNOWN;
/**
 * @var int
 * @cname XML_ERR_ERROR
 */
const LIBXML_ERR_ERROR = UNKNOWN;
/**
 * @var int
 * @cname XML_ERR_FATAL
 */
const LIBXML_ERR_FATAL = UNKNOWN;

class LibXMLError
{
    public int $level;
    public int $code;
    public int $column;
    public string $message;
    public string $file;
    public int $line;
}

/** @param resource $context */
function libxml_set_streams_context($context): void {}

function libxml_use_internal_errors(?bool $use_errors = null): bool {}

function libxml_get_last_error(): LibXMLError|false {}

/** @return array<int, LibXMLError> */
function libxml_get_errors(): array {}

function libxml_clear_errors(): void {}

/** @deprecated */
function libxml_disable_entity_loader(bool $disable = true): bool {}

function libxml_set_external_entity_loader(?callable $resolver_function): bool {}
