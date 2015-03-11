--TEST--
Bug #69212: Leaking VIA_HANDLER func when exception thrown in __call/... arg passing
--FILE--
<?php

class Test {
    public static function __callStatic($method, $args) {}
    public function __call($method, $args) {}
}

function do_throw() { throw new Exception; }

try {
    Test::foo(do_throw());
} catch (Exception $e) {
    echo "Caught!\n";
}
try {
    (new Test)->bar(do_throw());
} catch (Exception $e) {
    echo "Caught!\n";
}

try {
	$f = function () {};
	$f->__invoke(do_throw());
} catch (Exception $e) {
	echo "Caught!\n";
}

try {
	$t = new Test;
	$f->__invoke($t->bar(Test::foo(do_throw())));
} catch (Exception $e) {
	echo "Caught!\n";
}

?>
--EXPECT--
Caught!
Caught!
Caught!
Caught!
