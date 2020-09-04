--TEST--
filter_input_array() and filter_var_array() with invalid $definition arguments
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
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
--EXPECT--
NULL
array(0) {
}
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
filter_input_array(): Argument #2 ($options) must be of type array|int|null, string given
filter_var_array(): Argument #2 ($options) must be of type array|int|null, string given
filter_input_array(): Argument #2 ($options) must be of type array|int|null, stdClass given
filter_var_array(): Argument #2 ($options) must be of type array|int|null, stdClass given
