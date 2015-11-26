--TEST--
Bug #70487: pack('x') produces an error
--FILE--
<?php

var_dump(pack('x') === "\0");

?>
--EXPECT--
bool(true)
