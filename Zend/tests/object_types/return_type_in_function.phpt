--TEST--
Adding a function object return type
--FILE--
<?php

function a() : object {
	return 12345;
}
a();
--EXPECTF--
Fatal error: Uncaught TypeError: Return value of a() must be an object, integer returned in %s:4
Stack trace:
#0 %s(6): a()
#1 {main}
  thrown in %s on line 4
