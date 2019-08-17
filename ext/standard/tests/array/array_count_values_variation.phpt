--TEST--
Test array_count_values() function : Test all normal parameter variations
--FILE--
<?php
/* Prototype  : proto array array_count_values(array input)
 * Description: Return the value as key and the frequency of that value in input as value
 * Source code: ext/standard/array.c
 * Alias to functions:
 */

/*
 * Test behaviour with parameter variations
 */

echo "*** Testing array_count_values() : parameter variations ***\n";

class A {
	static function hello() {
	  echo "Hello\n";
	}
}

$ob = new A();

$fp = fopen("array_count_file", "w+");

$arrays = array ("bobk" => "bobv", "val", 6 => "val6",  $fp, $ob);

try {
    var_dump (array_count_values ($arrays));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}

?>

DONE
--CLEAN--
<?php
unlink("array_count_file");
?>
--EXPECT--
*** Testing array_count_values() : parameter variations ***
Can only count STRING and INTEGER values!

DONE
