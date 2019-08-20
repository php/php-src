--TEST--
basic array_count_values test
--FILE--
<?php
$array1 = array(1,
				"hello",
				1,
				"world",
				"hello",
				"",
				"rabbit",
				"foo",
				"Foo",
				TRUE,
				FALSE,
				NULL,
				0);

try {
    var_dump(array_count_values($array1));
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Can only count STRING and INTEGER values!
