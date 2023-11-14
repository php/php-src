--TEST--
JIT SEND_VAL: 002 named arg 
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
--FILE--
<?php
function o(){
    var_dump(x:$x?1:0);
}
o();
?>
--EXPECTF--
Warning: Undefined variable $x in %ssend_val_002.php on line 3

Fatal error: Uncaught ArgumentCountError: var_dump() expects at least 1 argument, 0 given in %ssend_val_002.php:3
Stack trace:
#0 %ssend_val_002.php(3): var_dump(x: 0)
#1 %ssend_val_002.php(5): o()
#2 {main}
  thrown in %ssend_val_002.php on line 3
