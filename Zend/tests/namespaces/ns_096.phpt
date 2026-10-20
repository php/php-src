--TEST--
Group use declaration list should not contain leading separator
--FILE--
<?php

use Foo\Bar\{\Baz};

?>
--EXPECTF--
Parse error: syntax error, unexpected fully qualified name "\Baz", expecting identifier or namespaced name or "function" or "const" in %s on line %d
