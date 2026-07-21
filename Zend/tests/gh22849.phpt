--TEST--
PHP 8.6 error_include_args=1 mishandled for include()
--INI--
error_include_args=On
--FILE--
<?php

function foo() {
    include("does not exist");
    include_once("does not exist");
    require("does not exist");
}

foo(1, 2);
?>
--EXPECTF--
Warning: include('does not exist'): Failed to open stream: No such file or directory in %s on line %d

Warning: include('does not exist'): Failed opening 'does not exist' for inclusion (include_path='%s') in %s on line %d

Warning: include_once('does not exist'): Failed to open stream: No such file or directory in %s on line %d

Warning: include_once('does not exist'): Failed opening 'does not exist' for inclusion (include_path='%s') in %s on line %d

Warning: require('does not exist'): Failed to open stream: No such file or directory in %s on line %d

Fatal error: Uncaught Error: Failed opening required 'does not exist' (include_path='%s') in %s:%d
Stack trace:
#0 %s(%d): foo(1, 2)
#1 {main}
  thrown in %s on line %d
