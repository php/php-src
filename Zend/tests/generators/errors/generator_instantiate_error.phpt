--TEST--
It's not possible to directly instantiate the Generator class
--FILE--
<?php

new Generator;

?>
--EXPECTF--
Fatal error: The "Generator" class is reserved for internal use and cannot be manually instantiated in %s on line %d
