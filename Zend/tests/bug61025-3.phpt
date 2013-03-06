--TEST--
Bug #61025 (__invoke() visibility not honored) raise warning when using static
--FILE--
<?php

interface IInvoke {
	public static function __invoke();
}

class Foo {
	public static function __invoke() {
		echo __CLASS__;
	}
}
?>
===DONE===
--EXPECTF--
Warning: The magic method __invoke() cannot be static in %s/bug61025-3.php on line %d

Warning: The magic method __invoke() cannot be static in %s/bug61025-3.php on line %d
===DONE===