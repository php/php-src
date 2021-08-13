--TEST--
filter_input_array() and filter_var_array() with invalid $definition arguments
--EXTENSIONS--
filter
--FILE--
<?php
foreach (array(null, true, false, 1, "", new stdClass) as $invalid) {
    try {
        var_dump(filter_input_array(INPUT_POST, $invalid));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }

    try {
        var_dump(filter_var_array(array(), $invalid));
    } catch (TypeError $exception) {
        echo $exception->getMessage() . "\n";
    }
}
?>
--EXPECTF--
Deprecated: filter_input_array(): Passing null to parameter #2 ($options) of type array|int is deprecated in %s on line %d

Warning: filter_input_array(): Unknown filter with ID 0 in %s on line %d
bool(false)

Deprecated: filter_var_array(): Passing null to parameter #2 ($options) of type array|int is deprecated in %s on line %d

Warning: filter_var_array(): Unknown filter with ID 0 in %s on line %d
bool(false)

Warning: filter_input_array(): Unknown filter with ID 1 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 1 in %s on line %d
bool(false)

Warning: filter_input_array(): Unknown filter with ID 0 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 0 in %s on line %d
bool(false)

Warning: filter_input_array(): Unknown filter with ID 1 in %s on line %d
bool(false)

Warning: filter_var_array(): Unknown filter with ID 1 in %s on line %d
bool(false)
filter_input_array(): Argument #2 ($options) must be of type array|int, string given
filter_var_array(): Argument #2 ($options) must be of type array|int, string given
filter_input_array(): Argument #2 ($options) must be of type array|int, stdClass given
filter_var_array(): Argument #2 ($options) must be of type array|int, stdClass given
