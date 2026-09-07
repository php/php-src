--TEST--
unserialize_callback_func with fully qualified name function
--INI--
unserialize_callback_func=\my_global_fn
--FILE--
<?php
function my_global_fn($name) {
	echo "callback_called\n";
    eval('class Foo {}');
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
