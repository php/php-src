--TEST--
return type with finally
--INI--
opcache.enable=0
--FILE--
<?php

function foo() : array {
	try {
		throw new Exception("xxxx");
	} finally {
		return null;
	}
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught Exception: xxxx in %s:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}

Next TypeError: Return value of foo() must be of the type array, null returned in %s29.php:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s029.php on line %d
