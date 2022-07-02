--TEST--
Test that mixed is a reserved class name
--FILE--
<?php

class mixed
{
}

?>
--EXPECTF--
Fatal error: Cannot use 'mixed' as class name as it is reserved in %s on line %d
