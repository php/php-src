--TEST--
preg_replace_callback() should not leak persistent memory on fatal error
--FILE--
<?php

function test() {}

preg_replace_callback('/a/', function($matches) {
    preg_replace_callback('/x/', function($matches) {
        function test() {} // Trigger a fatal error.
        return 'y';
    }, 'x');
    return 'b';
}, 'a');

?>
--EXPECTF--
Fatal error: Cannot redeclare test() (previously declared in %s on line %d
