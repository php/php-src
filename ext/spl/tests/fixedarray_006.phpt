--TEST--
SPL: FixedArray: Assigning objects
--FILE--
<?php

$b = 10000;
$a = new SplFixedArray($b);

try {
	for ($i = 0; $i < 100; $i++) {
		$a[] = new stdClass;
	}
} catch (\ValueError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Index invalid or out of range
