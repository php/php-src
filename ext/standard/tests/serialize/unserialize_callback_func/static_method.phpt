--TEST--
unserialize_callback_func with public static method
--INI--
unserialize_callback_func=Tester::my_unserialize
--FILE--
<?php

class Tester {
	public static function my_unserialize($name) {
    	echo "callback_called\n";
    	eval('class Foo {}');
    }
}

$o = unserialize('O:3:"FOO":0:{}');

var_dump($o);

echo "Done";

?>
--EXPECT--
callback_called
object(Foo)#1 (0) {
}
Done
