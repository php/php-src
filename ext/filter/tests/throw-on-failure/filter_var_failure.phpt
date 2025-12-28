--TEST--
FILTER_THROW_ON_FAILURE: filter_input() failure
--EXTENSIONS--
filter
--FILE--
<?php

echo "\nvalidation fails (array type check)\n";
try {
	filter_var('a', FILTER_DEFAULT, FILTER_REQUIRE_ARRAY | FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nvalidation fails (object without __toString)\n";
try {
	filter_var(new stdClass(), FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nvalidation fails (filter value)\n";
try {
	filter_var('a', FILTER_VALIDATE_EMAIL, FILTER_THROW_ON_FAILURE);
} catch (Filter\FilterFailedException $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
?>
--EXPECT--
validation fails (array type check)
Filter\FilterFailedException: filter validation failed: not an array (got string)

validation fails (object without __toString)
Filter\FilterFailedException: filter validation failed: object of type stdClass has no __toString() method

validation fails (filter value)
Filter\FilterFailedException: filter validation failed: filter validate_email not satisfied by 'a'
