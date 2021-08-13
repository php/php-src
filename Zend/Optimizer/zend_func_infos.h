/* This is a generated file, edit the .stub.php files instead. */

static const func_info_t func_infos[] = {
    F1("zend_version", MAY_BE_STRING),
    FN("func_get_args", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_ANY),
    F1("get_class_methods", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    F1("get_included_files", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING),
    FN("set_error_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT|MAY_BE_NULL),
    FN("set_exception_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT|MAY_BE_NULL),
    F1("get_resource_type", MAY_BE_STRING),
    F1("get_defined_constants", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_ANY),
    FN("constant", MAY_BE_ANY),
#if HAVE_NANOSLEEP
    F1("time_nanosleep", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_STRING|MAY_BE_ARRAY_OF_LONG|MAY_BE_BOOL),
#endif
#if defined(PHP_WIN32) || HAVE_DNS_SEARCH_FUNC
    F1("dns_get_record", MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_ARRAY|MAY_BE_FALSE),
#endif
    FN("opendir", MAY_BE_RESOURCE|MAY_BE_FALSE),
    F1("popen", MAY_BE_RESOURCE|MAY_BE_FALSE),
};
