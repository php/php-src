--TEST--
$v = !$v/(bool)$v checks for undefined variables
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function undef_negation() {
    echo "In undef_negation\n";
    $v = !$v;
    var_export($v);
    echo "\n";
}
function undef_bool_cast() {
    echo "In undef_bool_cast\n";
    $v = (bool)$v;
    var_export($v);
    echo "\n";
}
undef_negation();
undef_bool_cast();
?>
--EXPECTF--
In undef_negation

Warning: Undefined variable $v in %s on line 4
true
In undef_bool_cast

Warning: Undefined variable $v in %s on line 10
false
