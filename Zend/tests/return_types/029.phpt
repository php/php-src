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
		return ;
	}
}

foo();
?>
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of foo() must be of the type array, none returned in %s29.php:%d
Stack trace:
#0 %s(%d): foo()
#1 {main}
  thrown in %s029.php on line %d
