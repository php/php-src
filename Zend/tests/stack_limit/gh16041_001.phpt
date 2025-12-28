--TEST--
GH-16041 001: Stack overflow in phpdbg
--SKIPIF--
<?php
if (ini_get('zend.max_allowed_stack_size') === false) {
    die('skip No stack limit support');
}
?>
--INI--
zend.max_allowed_stack_size=512K
--PHPDBG--
set pagination off
run
continue
quit
--FILE--
<?php

class Canary {
    public function __destruct() {
        new Canary();
    }
}

new Canary();

?>
--EXPECTF--
[Successful compilation of %sgh16041_001.php]
prompt> prompt> [Uncaught Error in %s on line %d: Maximum call stack size of %d bytes%s
>00005:         new Canary();
 00006:     }
 00007: }
prompt> [Uncaught Error in %s on line %d]
Error: Maximum call stack size of %d bytes (zend.max_allowed_stack_size - zend.reserved_stack_size) reached. Infinite recursion? in %s:%d
Stack trace:
#0 %s(%d): Canary->__destruct()
%a
prompt>
