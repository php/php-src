<?php

function ffi_get_fastcall_specifier()
{
    foreach (['__attribute__((fastcall))', '__fastcall', '__vectorcall'] as $spec) {
        try {
            FFI::cdef("extern size_t $spec zend_list_insert(void *ptr, int type);");
            return "$spec ";
        } catch (Throwable $e) {}
    }
    return "";
}
