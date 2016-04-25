--TEST--
intersection test
--FILE--
<?php
interface IFoo {
	public function iFoo();
}

interface IBar {
	public function iBar();
}

class Foo implements IFoo {
	public function iFoo() {}
}

class Bar implements IFoo, IBar {
	public function iFoo() {}
	public function iBar() {}
}

/* not sure if string should be check first? */
$cb = function(IFoo & IBar $arg) {
	return $arg;
};

var_dump($cb(new Bar));

$cb(new Foo);
?>
--EXPECTF--
object(Bar)#2 (0) {
}

Fatal error: Uncaught TypeError: Argument 1 passed to {closure}() must be IFoo and IBar, instance of Foo given, called in %s on line 26 and defined in %s:20
Stack trace:
#0 %s(26): {closure}(Object(Foo))
#1 {main}
  thrown in %s on line 20
