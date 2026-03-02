--TEST--
Bug #66176 (Invalid constant substitution)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($v) {
    global $a;
    return $a[$v];
}
$a = array(PHP_VERSION => 1);
var_dump(foo(PHP_VERSION));
?>
--EXPECT--
int(1)
