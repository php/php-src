--TEST--
AST pretty-printer
--INI--
zend.assertions=1
assert.exception=0
--FILE--
<?php
assert(0 && ($a = function () {
	$var = 'test';
	$str = "$var, $var[1], {$var}[], {$var[1]}[], ${var}[], ${var[1]}[]";
}));
?>
--EXPECTF--
Warning: assert(): assert(0 && ($a = function () {
    $var = 'test';
    $str = "$var, {$var[1]}, {$var}[], {$var[1]}[], {$var}[], {$var[1]}[]";
})) failed in %sexpect_020.php on line %d
