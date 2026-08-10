--TEST--
unserialize_callback_func with partially deprecated callable string
--INI--
unserialize_callback_func=parent::my_unserialize
--FILE--
<?php

class TesterParent {
	public static function my_unserialize($name) {
    	echo 'callback_called in ', __CLASS__ , PHP_EOL;
    	eval('class Foo {}');
    }
}

class TesterChild extends TesterParent {
	public static function my_unserialize($name) {
    	echo 'callback_called in ', __CLASS__ , PHP_EOL;
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
	$tester = new TesterChild();
	$o = $tester->unserialize($s);
	var_dump($o);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
echo "Done";

?>
--EXPECTF--
Error: Invalid callback parent::my_unserialize, cannot access "parent" when no class scope is active

Deprecated: Use of "parent" in callables is deprecated in %s on line %d
callback_called in TesterParent
object(Foo)#3 (0) {
}
Done
