--TEST--
JIT ASSIGN: 032
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
$var = "intergalactic";
$var1 = "space";
$var2 = &$var1;
$var = $var2;
var_dump($var);
var_dump($var1);
var_dump($var2);
echo "Done\n";
?>
--EXPECT--
string(5) "space"
string(5) "space"
string(5) "space"
Done
