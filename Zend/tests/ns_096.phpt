--TEST--
Group use declaration list should not contain leading separator
--FILE--
<?php

use Foo\Bar\{\Baz};

?>
--EXPECTF--
Parse error: syntax error, unexpected '\Baz' (T_NAME_FULLY_QUALIFIED), expecting identifier (T_STRING) or namespaced name (T_NAME_QUALIFIED) or function (T_FUNCTION) or const (T_CONST) in %s on line %d
