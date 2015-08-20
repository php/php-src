--TEST--
Group use declaration list should not contain leading separator
--FILE--
<?php

use Foo\Bar\{\Baz};

?>
--EXPECTF--
Parse error: syntax error, unexpected '\' (T_NS_SEPARATOR), expecting identifier (T_STRING) or function (T_FUNCTION) or const (T_CONST) in /home/nikic/php-src/Zend/tests/ns_096.php on line 3
