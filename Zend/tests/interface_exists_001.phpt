--TEST--
Testing interface_exists()
--FILE--
<?php

interface foo {
}

var_dump(interface_exists('foo'));
var_dump(interface_exists(1));

?>
--EXPECT--
bool(true)
bool(false)
