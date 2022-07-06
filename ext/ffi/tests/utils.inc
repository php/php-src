<?php

function ffi_cdef($code, $lib)
{
    if (isset($lib)) {
        return FFI::cdef($code, $lib);
    } else {
        return FFI::cdef($code);
    }
}

function ffi_get_php_dll_name()
{
    if (PHP_OS_FAMILY === 'Windows') {
        return "php" . PHP_MAJOR_VERSION . (PHP_ZTS ? "ts" : "") . (PHP_DEBUG ? "_debug" : "") . ".dll";
    } else {
        return null;
    }
}

function ffi_get_fastcall_specifier()
{
    foreach (['__attribute__((fastcall))', '__fastcall', '__vectorcall'] as $spec) {
        try {
            ffi_cdef("extern size_t $spec zend_list_insert(void *ptr, int type);", ffi_get_php_dll_name());
            return "$spec ";
        } catch (Throwable $e) {}
    }
    return "";
}
