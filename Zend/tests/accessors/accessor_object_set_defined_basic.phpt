--TEST--
ZE2 Tests that an defined setter has a protected auto-implemented variable defined and that it can be changed through the accessor
--FILE--
<?php

class AccessorTest {
	public $b {
		set {
			echo "Setter called.\n"; 
		}
	}
}

$o = new AccessorTest();

$o->b = 10;
echo "Done\n";
?>
--EXPECTF--
Setter called.
Done
