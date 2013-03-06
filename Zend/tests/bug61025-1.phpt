--TEST--
Bug #61025 (__invoke() visibility not honored) invoke private
--FILE--
<?php

class FooPrivate {
	private function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}
$foo_private = new FooPrivate();

$foo_private();
?>
===DONE===
--EXPECTF--
Fatal error: Call to private FooPrivate::__invoke() from context '' in %s/bug61025-1.php on line %d