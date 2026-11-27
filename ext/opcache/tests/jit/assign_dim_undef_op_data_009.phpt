--TEST--
JIT ASSIGN_DIM: result is null when the container converted from false is copied while reporting undefined OP_DATA
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=function
--FILE--
<?php
set_error_handler(function ($_, $msg) {
    echo $msg, "\n";
    if (str_starts_with($msg, 'Undefined variable')) {
        $GLOBALS['copy'] = debug_backtrace()[1]['args'][0];
    }
});
function test($x) {
    $y = ($x[0] = $undef);
    var_dump($y);
}
test(false);
?>
--EXPECT--
Automatic conversion of false to array is deprecated
Undefined variable $undef
NULL
