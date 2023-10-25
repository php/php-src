--TEST--
Switch with undefined input
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--FILE--
<?php
function foo() {
    switch($y) {
        case 'x':
        case 'y':
    }
}
foo();
?>
DONE
--EXPECTF--
Warning: Undefined variable $y in %sswitch_001.php on line 4

Warning: Undefined variable $y in %sswitch_001.php on line 5
DONE
