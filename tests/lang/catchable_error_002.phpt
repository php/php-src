--TEST--
Catchable fatal error [2]
--FILE--
<?php
	class Foo {
	}

	function blah (Foo $a)
	{
	}

	function error()
	{
		$a = func_get_args();
		var_dump($a);
	}

	set_error_handler('error');

	blah (new StdClass);
	echo "ALIVE!\n";
?>
--EXPECTF--
array(5) {
  [0]=>
  int(4096)
  [1]=>
  string(%d) "Argument 1 passed to blah() must be an instance of Foo, instance of stdClass given, called in %scatchable_error_002.php on line %d and defined"
  [2]=>
  string(%d) "%scatchable_error_002.php"
  [3]=>
  int(5)
  [4]=>
  array(0) {
  }
}
ALIVE!
