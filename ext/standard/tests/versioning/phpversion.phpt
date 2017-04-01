--TEST--
phpversion test
--FILE--
<?php

var_dump(phpversion());
var_dump(phpversion('standard'));

--EXPECTF--
string(%d) "%s"
string(%d) "%s"
