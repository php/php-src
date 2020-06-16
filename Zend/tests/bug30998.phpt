--TEST--
Bug #30998 (Crash when user error handler returns false)
--FILE--
<?php
error_reporting(-1);

function my_error($errno, $errstr, $errfile, $errline)
{
        print "$errstr ($errno) in $errfile:$errline\n";
        return false;
}
set_error_handler('my_error');

$f = fopen("/tmp/blah", "r");
?>
--EXPECTF--
fopen(/tmp/blah): Failed to open stream: %s (2) in %s:%d

Warning: fopen(/tmp/blah): Failed to open stream: %s in %s on line %d
