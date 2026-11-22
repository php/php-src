--TEST--
JIT ASSIGN_DIM: container converted from false is overwritten while reporting undefined OP_DATA
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
        $GLOBALS['x'] = 1;
    }
});
function test() {
    global $x;
    $x = false;
    $x[0] = $undef;
}
test();
var_dump($x);
?>
--EXPECT--
Automatic conversion of false to array is deprecated
Undefined variable $undef
int(1)
