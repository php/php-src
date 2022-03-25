--TEST--
PRE_INC_OBJ: 005
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
opcache.protect_memory=1
--FILE--
<?php
json_encode($y)->y++;
?>
--EXPECTF--
Warning: Undefined variable $y in %sinc_obj_005.php on line 2

Fatal error: Uncaught Error: Attempt to increment/decrement property "y" on string in %sinc_obj_005.php:2
Stack trace:
#0 {main}
  thrown in %sinc_obj_005.php on line 2
