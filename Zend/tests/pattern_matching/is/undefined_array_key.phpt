--TEST--
Pattern matching: Undefined index should not match null
--FILE--
<?php

var_dump(['a' => 'a'] is ['b' => null]);

?>
--EXPECT--
bool(false)
