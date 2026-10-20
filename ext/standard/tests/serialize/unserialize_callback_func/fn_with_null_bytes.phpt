--TEST--
unserialize_callback_func with function name containing null bytes
--FILE--
<?php

ini_set('unserialize_callback_func', "foo\0butno");

function foo(string $name) {
	echo "callback_called\n";
    eval('class Foo {}');
}

$s = 'O:3:"FOO":0:{}';
try {
	$o = unserialize($s);
	var_dump($o);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Done";
?>
--EXPECT--
Error: Invalid callback foo, function "foo" not found or invalid function name
Done
