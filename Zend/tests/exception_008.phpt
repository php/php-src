--TEST--
Exception in __destruct while exception is pending
--FILE--
<?php

class TestFirst
{
	function __destruct() {
		throw new Exception("First");
	}
}

class TestSecond
{
	function __destruct() {
		throw new Exception("Second");		
	}
}

$ar = array(new TestFirst, new TestSecond);

unset($ar);

?>
===DONE===
--EXPECTF--
Fatal error: Uncaught exception 'Exception' with message 'First' in %sexception_008.php:%d
Stack trace:
#0 %sexception_008.php(%d): TestFirst->__destruct()
#1 {main}

Next exception 'Exception' with message 'Second' in %sexception_008.php:%d
Stack trace:
#0 %sexception_008.php(%d): TestSecond->__destruct()
#1 {main}
  thrown in %sexception_008.php on line %d
