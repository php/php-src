--TEST--
unserialize_callback_func with fully qualified named namespaced function
--INI--
unserialize_callback_func=\php\test\my_global_fn
--FILE--
<?php

namespace php\test {
	function my_global_fn($name) {
		echo "callback_called\n";
    	eval('class Foo {}');
	}
}
namespace {
	$o = unserialize('O:3:"FOO":0:{}');

	var_dump($o);

	echo "Done";
}
?>
--EXPECT--
callback_called
object(Foo)#1 (0) {
}
Done
