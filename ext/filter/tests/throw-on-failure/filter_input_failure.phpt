--TEST--
FILTER_THROW_ON_FAILURE: filter_input() failure
--EXTENSIONS--
filter
--GET--
a=1
--FILE--
<?php

echo "missing value\n";
try {
	filter_input(INPUT_GET, 'b', FILTER_DEFAULT, FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nvalidation fails (array type check)\n";
try {
	filter_input(INPUT_GET, 'a', FILTER_DEFAULT, FILTER_REQUIRE_ARRAY | FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nvalidation fails (filter value)\n";
try {
	filter_input(INPUT_GET, 'a', FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
?>
--EXPECT--
missing value
Filter\FilterFailedException: input value 'b' not found

validation fails (array type check)
Filter\FilterFailedException: filter validation failed: not an array (got string)

validation fails (filter value)
Filter\FilterFailedException: filter validation failed: filter validate_email not satisfied by '1'
