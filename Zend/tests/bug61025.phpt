--TEST--
Bug #61025 (__invoke() visibility not honored)
--FILE--
<?php

class FooPublic {
	public function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}

class FooProtected {
	protected function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}

$closure = function () {
	echo "Closure\n";
};

$foo_public = new FooPublic();
$foo_protected = new FooProtected();

$closure();
$foo_public();
$foo_protected();
?>
===DONE===
--EXPECTF--
Closure
FooPublic::__invoke()

Fatal error: Call to protected FooProtected::__invoke() from context '' in %s/bug61025.php on line %d