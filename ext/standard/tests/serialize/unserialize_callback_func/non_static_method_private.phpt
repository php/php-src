--TEST--
unserialize_callback_func with private non-static method
--INI--
unserialize_callback_func=Tester::my_unserialize
--FILE--
<?php

class Tester {
	private function my_unserialize($name) {
    	echo "callback_called\n";
    	eval('class Foo {}');
    }

    public function unserialize(string $str) {
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
	$tester = new Tester();
	$o = $tester->unserialize($s);
	var_dump($o);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Done";

?>
--EXPECT--
Error: Invalid callback Tester::my_unserialize, non-static method Tester::my_unserialize() cannot be called statically
callback_called
object(Foo)#3 (0) {
}
Done
