--TEST--
unserialize_callback_func with private static method
--INI--
unserialize_callback_func=Tester::my_unserialize
--FILE--
<?php

class Tester {
	private static function my_unserialize($name) {
    	echo "callback_called\n";
    	eval('class Foo {}');
    }

    public static function unserialize(string $str) {
    	return unserialize($str);
    }
}

$s = 'O:3:"FOO":0:{}';
try {
	$o = unserialize($s);
	var_dump($o);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
	$o = Tester::unserialize($s);
	var_dump($o);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Done";

?>
--EXPECT--
Error: Invalid callback Tester::my_unserialize, cannot access private method Tester::my_unserialize()
callback_called
object(Foo)#2 (0) {
}
Done
