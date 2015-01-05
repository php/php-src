--TEST--
Cannot use special class name as trait name
--FILE--
<?php

trait self {}

?>
--EXPECTF--
Fatal error: Cannot use 'self' as class name as it is reserved in %s on line %d
