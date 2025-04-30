--TEST--
JIT CONCAT: 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
function foo($a, $b) {
	return $a . $b;
}
var_dump(foo("a", "b"));
var_dump(foo("a", 5));
?>
--EXPECT--
string(2) "ab"
string(2) "a5"
