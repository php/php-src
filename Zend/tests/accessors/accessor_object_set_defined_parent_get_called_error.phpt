--TEST--
ZE2 Tests that access to a undefined parent getter produces an error
--FILE--
<?php

class ParentAccessorTest {
	public $b {
		set {
			echo __METHOD__."($value)".PHP_EOL;
		}
	}
}

class AccessorTest extends ParentAccessorTest {
	public $b {
		get {
			echo __METHOD__.'()'.PHP_EOL;
			return (new ReflectionProperty(get_parent_class(), 'b'))->getValue($this);
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
AccessorTest::$b->get()

Warning: Cannot get property ParentAccessorTest::$b, no getter defined in %s on line %d
