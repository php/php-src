--TEST--
JIT NOT: 003
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
set_error_handler(function(){y;}) . !$y;
?>
--EXPECTF--
Fatal error: Uncaught Error: Undefined constant "y" in %snot_003.php:2
Stack trace:
#0 %snot_003.php(2): {closure}(2, '%s', '%s', 2)
#1 {main}
  thrown in %snot_003.php on line 2