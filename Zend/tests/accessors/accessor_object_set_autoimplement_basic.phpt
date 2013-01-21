--TEST--
ZE2 Tests that an auto-implemented setter has an auto-implemented variable defined and that it can be changed through the accessor
--FILE--
<?php

class AccessorTest {
	public $b {
		set;
	}

	public function __construct() {
		$this->b = 5;
	}
}

$o = new AccessorTest();

print_r($o);
$o->b = 10;
print_r($o);
echo "Done\n";
?>
--EXPECTF--
AccessorTest Object
(
    [b] => 5
)
AccessorTest Object
(
    [b] => 10
)
Done
