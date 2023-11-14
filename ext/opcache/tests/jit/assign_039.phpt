--TEST--
JIT ASSIGN: Assign reference IS_VAR
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
$a = $ref =& $val;
var_dump($a);
?>
--EXPECT--
NULL
