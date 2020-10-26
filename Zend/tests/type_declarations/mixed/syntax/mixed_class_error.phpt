--TEST--
Test that mixed is a reserved class name
--FILE--
<?php

class mixed
{
}

?>
--EXPECTF--
Fatal error: Class "mixed" cannot be declared, mixed is a reserved class name in %s on line %d
