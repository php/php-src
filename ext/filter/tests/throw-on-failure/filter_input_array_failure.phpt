--TEST--
FILTER_THROW_ON_FAILURE: filter_input_array() failure
--EXTENSIONS--
filter
--GET--
a=1
--FILE--
<?php

echo "\nvalidation fails (array type check)\n";
try {
	filter_input_array(INPUT_GET, ['a' => ['flags' => FILTER_REQUIRE_ARRAY | FILTER_THROW_ON_FAILURE]]);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nvalidation fails (filter value)\n";
try {
	filter_input_array(INPUT_GET, ['a' => ['filter' => FILTER_VALIDATE_EMAIL, 'flags' => FILTER_THROW_ON_FAILURE]]);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
?>
--EXPECT--
validation fails (array type check)
Filter\FilterFailedException: filter validation failed: not an array (got string)

validation fails (filter value)
Filter\FilterFailedException: filter validation failed: filter validate_email not satisfied by '1'
