--TEST--
JIT COPY_TMP: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--FILE--
<?php
$x[~"abc"] ??= 0;
?>
DONE
--EXPECT--
DONE