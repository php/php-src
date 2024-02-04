--TEST--
JIT ASSIGN: Assign refcounted string (with result)
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
$n = 2;
$a = $b = str_repeat('a', $n);
var_dump($a, $b);
?>
--EXPECT--
string(2) "aa"
string(2) "aa"
