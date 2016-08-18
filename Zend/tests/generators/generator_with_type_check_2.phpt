--TEST--
Generator wit type check
--FILE--
<?php
function gen(array $a) { yield; }
try {
	gen(42);
} catch (TypeError $e) {
	echo $e->getMessage()."\n";
}

try {
	foreach (gen(42) as $val) {
		var_dump($val);
	}
} catch (TypeError $e) {
        echo $e->getMessage()."\n";
}
?>
--EXPECTF--
Argument 1 passed to gen() must be of the type array, integer given, called in %sgenerator_with_type_check_2.php on line 4
Argument 1 passed to gen() must be of the type array, integer given, called in %sgenerator_with_type_check_2.php on line 10
