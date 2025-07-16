--TEST--
Cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE
--EXTENSIONS--
filter
--GET--
a=1
--FILE--
<?php

$flags = FILTER_THROW_ON_FAILURE | FILTER_NULL_ON_FAILURE;

echo "filter_input(), with a missing value\n";
try {
	filter_input(INPUT_GET, 'b', FILTER_DEFAULT, $flags);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
try {
	filter_input(INPUT_GET, 'b', FILTER_DEFAULT, ['flags' => $flags]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
echo "\nfilter_input(), with a missing value and a default\n";
try {
	filter_input(INPUT_GET, 'b', FILTER_DEFAULT, ['flags' => $flags, 'options' => ['default' => 'a']]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
echo "\nfilter_input(), with a present value\n";
try {
	filter_input(INPUT_GET, 'a', FILTER_DEFAULT, $flags);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
try {
	filter_input(INPUT_GET, 'a', FILTER_DEFAULT, ['flags' => $flags]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
echo "\nfilter_var()\n";
try {
	filter_var(true, FILTER_DEFAULT, $flags);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
try {
	filter_var(true, FILTER_DEFAULT, ['flags' => $flags]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nfilter_input_array()\n";
try {
	filter_input_array(INPUT_GET, ['a' => ['flags' => $flags]]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}

echo "\nfilter_var_array()\n";
try {
	filter_var_array(['a' => true], ['a' => ['flags' => $flags]]);
} catch (ValueError $e) {
	echo get_class($e) . ": " . $e->getMessage() . "\n";
}
?>
--EXPECT--
filter_input(), with a missing value
ValueError: filter_input(): Argument #4 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE
ValueError: filter_input(): Argument #4 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE

filter_input(), with a missing value and a default
ValueError: filter_input(): Argument #4 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE

filter_input(), with a present value
ValueError: filter_input(): Argument #4 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE
ValueError: filter_input(): Argument #4 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE

filter_var()
ValueError: filter_var(): Argument #3 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE
ValueError: filter_var(): Argument #3 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE

filter_input_array()
ValueError: filter_input_array(): Argument #2 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE

filter_var_array()
ValueError: filter_var_array(): Argument #2 ($options) cannot use both FILTER_NULL_ON_FAILURE and FILTER_THROW_ON_FAILURE
