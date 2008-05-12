--TEST--
Testing callback in alias
--FILE--
<?php

class foo { 
	static public function test() {
		print "hello\n";
	}
	public function test2() {
		print "foobar!\n";
	}
}

class_alias('FOO', 'bar');

call_user_func(array('bar', 'test'));


$a = new bar;
call_user_func(array($a, 'test2'));

?>
--EXPECT--
hello
foobar!
