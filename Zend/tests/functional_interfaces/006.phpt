--TEST--
functional interfaces: interface call
--FILE--
<?php
interface IFoo {
	public function method(string $arg) : void;
}

$foo = function (string $arg) implements IFoo : void {
	var_dump($arg);
};

$foo->method("turtles ?");
--EXPECTF--
string(9) "turtles ?"




