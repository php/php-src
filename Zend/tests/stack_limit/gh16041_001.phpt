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
prompt> prompt> [Script ended normally]
prompt> [Not running]
prompt>
