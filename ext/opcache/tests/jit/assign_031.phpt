--TEST--
JIT ASSIGN: 031
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
;opcache.jit_debug=257
--EXTENSIONS--
opcache
--FILE--
<?php
$c =& $a;
$b = $a;
echo "ok\n";
?>
--EXPECT--
ok
