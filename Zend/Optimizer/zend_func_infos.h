static const func_info_t func_infos[] = {
    F0("Exception::getCode", MAY_BE_LONG),
    F0("Error::getCode", MAY_BE_LONG),
    F1("zend_version", MAY_BE_STRING),
    FN("func_get_args", MAY_BE_ARRAY|MAY_BE_LONG|MAY_BE_ANY),
    F0("set_error_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_LONG|MAY_BE_ANY|MAY_BE_OBJECT|MAY_BE_NULL),
    F0("set_exception_handler", MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_STRING|MAY_BE_LONG|MAY_BE_ANY|MAY_BE_OBJECT|MAY_BE_NULL),
};
