--TEST--
Throw reference
--FILE--
<?php

$e = new Exception;
$ref =& $e;
throw $e;

?>
--EXPECTF--
Exception: (empty message) in %s on line %d
Stack trace:
#0 {main}
