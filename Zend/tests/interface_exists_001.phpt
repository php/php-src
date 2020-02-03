--TEST--
Testing interface_exists()
--FILE--
<?php

interface foo
{
}

var_dump(interface_exists('foo'));
var_dump(interface_exists(1));
var_dump(interface_exists(null));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
