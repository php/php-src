--TEST--
OSS-Fuzz #410939023: Use of magic const within const expr cast
--FILE--
<?php

const C = (string)__METHOD__;
var_dump(C);

?>
--EXPECT--
string(0) ""
