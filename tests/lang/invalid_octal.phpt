--TEST--
Invalid octal
--FILE--
<?php

$x = 08;
--EXPECTF--
Fatal error: Invalid numeric literal in %s on line 3
