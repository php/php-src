--TEST--
GH-9090 (Support assigning function pointers via FFI)
--EXTENSIONS--
ffi
zend_test
--FILE--
<?php

require_once 'utils.inc';
$h = <<<'EOD'
void (*bug_gh9090_void_none_ptr)();
void (*bug_gh9090_void_int_char_ptr)(int, char *);
void (*bug_gh9090_void_int_char_var_ptr)(int, char *, ...);
void (*bug_gh9090_void_char_int_ptr)(char *, int);
int (*bug_gh9090_int_int_char_ptr)(int, char *);

void bug_gh9090_void_none();
void bug_gh9090_void_int_char(int i, char *s);
void bug_gh9090_void_int_char_var(int i, char *fmt, ...);
EOD;

if (PHP_OS_FAMILY !== 'Windows') {
    $ffi = FFI::cdef($h);
} else {
    try {
        $ffi = FFI::cdef($h, 'php_zend_test.dll');
    } catch (FFI\Exception $ex) {
        $ffi = FFI::cdef($h, ffi_get_php_dll_name());
    }
}

$func_ptrs = [
    'bug_gh9090_void_none_ptr',
    'bug_gh9090_void_int_char_ptr',
    'bug_gh9090_void_int_char_var_ptr',
    'bug_gh9090_void_char_int_ptr',
    'bug_gh9090_int_int_char_ptr',
];

$func_argvs = [
    [ 'bug_gh9090_void_none',         [ ]                           ],
    [ 'bug_gh9090_void_int_char',     [ 42, "hello" ]               ],
    [ 'bug_gh9090_void_int_char_var', [ 42, "d=%d s=%s", -1, "ok" ] ],
];

foreach ($func_ptrs as $func_ptr) {
    foreach ($func_argvs as $func_argv) {
        [ $func, $argv ] = $func_argv;

        $ok = true;
        try {
            $ffi->$func_ptr = $ffi->$func;
            call_user_func_array($ffi->$func_ptr, $argv);
        } catch (FFI\Exception $e) {
            $ok = false;
        }

        printf("%-36s = %-36s ? %s\n", $func_ptr, $func, $ok ? 'yes' : 'no');
    }
}
?>
--EXPECT--
bug_gh9090_none
bug_gh9090_void_none_ptr             = bug_gh9090_void_none                 ? yes
bug_gh9090_void_none_ptr             = bug_gh9090_void_int_char             ? no
bug_gh9090_void_none_ptr             = bug_gh9090_void_int_char_var         ? no
bug_gh9090_void_int_char_ptr         = bug_gh9090_void_none                 ? no
bug_gh9090_int_char 42 hello
bug_gh9090_void_int_char_ptr         = bug_gh9090_void_int_char             ? yes
bug_gh9090_void_int_char_ptr         = bug_gh9090_void_int_char_var         ? no
bug_gh9090_void_int_char_var_ptr     = bug_gh9090_void_none                 ? no
bug_gh9090_void_int_char_var_ptr     = bug_gh9090_void_int_char             ? no
bug_gh9090_void_int_char_var d=-1 s=ok
bug_gh9090_void_int_char_var_ptr     = bug_gh9090_void_int_char_var         ? yes
bug_gh9090_void_char_int_ptr         = bug_gh9090_void_none                 ? no
bug_gh9090_void_char_int_ptr         = bug_gh9090_void_int_char             ? no
bug_gh9090_void_char_int_ptr         = bug_gh9090_void_int_char_var         ? no
bug_gh9090_int_int_char_ptr          = bug_gh9090_void_none                 ? no
bug_gh9090_int_int_char_ptr          = bug_gh9090_void_int_char             ? no
bug_gh9090_int_int_char_ptr          = bug_gh9090_void_int_char_var         ? no
