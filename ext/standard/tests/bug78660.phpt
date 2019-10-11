--TEST--
Bug #78660	rounding negative zero
--FILE--
<?php
var_dump((string) round((float) "-0", 2));
--EXPECT--
string(1) "0"
