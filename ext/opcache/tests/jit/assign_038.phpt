--TEST--
JIT ASSIGN: Assign constant (with result)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
opcache.optimization_level=0 ; disable optimizer to produce ASSIGN with result
--EXTENSIONS--
opcache
--FILE--
<?php
$a = $b = "bb";
var_dump($a, $b);
?>
--EXPECT--
string(2) "bb"
string(2) "bb"
