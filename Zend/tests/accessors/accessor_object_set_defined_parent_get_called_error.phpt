--TEST--
ZE2 Tests that access to a undefined parent getter produces an error
--FILE--
<?php

class ParentAccessorTest {
	public $a {
		set {
			echo "set ParentAccessorTest::\$a Called \$value = {$value}.\n";
		}
	}
}

class AccessorTest extends ParentAccessorTest {
	public $b {
		get {
			echo "get AccessorTest::\$b Called.\n";
			return parent::$a;
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
Fatal error: Cannot get property parent::$a, no getter defined. in %s on line %d