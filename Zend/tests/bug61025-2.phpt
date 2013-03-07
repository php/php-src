--TEST--
Bug #61025 (__invoke() visibility not honored) is_callable consist
--FILE--
<?php

class FooPublic {
	public function __construct() {
		echo "Check in class: " . __CLASS__ . " \$this is callable: ";
		echo is_callable($this) ? 'true' : 'false';
		echo "\n";
	}
	public function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}

class FooProtected {
	public function __construct() {
		echo "Check in class: " . __CLASS__ . " \$this is callable: ";
		echo is_callable($this) ? 'true' : 'false';
		echo "\n";
	}

	protected function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}

class FooPrivate {
	public function __construct() {
		echo "Check in class: " . __CLASS__ . " \$this is callable: ";
		echo is_callable($this) ? 'true' : 'false';
		echo "\n";
	}

	private function __invoke() {
		echo __CLASS__ . "::" . __FUNCTION__ . "()\n";
	}
}

$foo_public = new FooPublic();
echo "Check outter is callable: ";
echo is_callable($foo_public) ? 'true' : 'false';
echo "\n\n";

$foo_protected = new FooProtected();
echo "Check outter is callable: ";
echo is_callable($foo_protected) ? 'true' : 'false';
echo "\n\n";

$foo_private = new FooPrivate();
echo "Check outter is callable: ";
echo is_callable($foo_private) ? 'true' : 'false';
echo "\n\n";
?>
===DONE===
--EXPECT--
Check in class: FooPublic $this is callable: true
Check outter is callable: true

Check in class: FooProtected $this is callable: true
Check outter is callable: false

Check in class: FooPrivate $this is callable: true
Check outter is callable: false

===DONE===