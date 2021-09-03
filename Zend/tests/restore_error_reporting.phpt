--TEST--
Test error_reporting being restored after fatal error during silencing
--FILE--
<?php

var_dump($undef_var);
@eval('class self {}');

?>
--EXPECTF--
Warning: Undefined variable $undef_var in %s on line %d
NULL

Fatal error: Cannot use 'self' as class name as it is reserved in %s on line %d
