--TEST--
An error should be generated when using ::class on a constant evaluated expression
--FILE--
<?php

(1+1)::class;

?>
--EXPECTF--
Fatal error: Cannot use "::class" on int in %s on line %d
