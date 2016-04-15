--TEST--
functional interfaces: binding
--FILE--
<?php
interface ILog {
	public function do() : void;
}

class Foo {}

$logger = function() implements ILog : void {
	var_dump($this instanceof Foo);
};

$foo = new Foo();

$bound = Closure::bind($logger, $foo);

var_dump($bound instanceof ILog);

$bound();
?>
--EXPECT--
bool(true)
bool(true)
