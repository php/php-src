--TEST--
Exit in argument list
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
var_dump(exit);
?>
--EXPECT--
