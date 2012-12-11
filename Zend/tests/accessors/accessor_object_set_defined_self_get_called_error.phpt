--TEST--
ZE2 Tests that access to a undefined self getter produces an error
--FILE--
<?php

class AccessorTest {
	public $a {
		set;
	}
	public $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return self::$a;
		}
		set {
			echo "Setter called.\n"; 
		}
	}
}

$o = new AccessorTest();
echo $o->b;
?>
--EXPECTF--
Fatal error: Cannot get property self::$a, no getter defined. in %s on line %d