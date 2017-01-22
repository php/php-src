--TEST--
filter_input_array() and filter_var_array() with invalid $definition arguments
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
foreach (array(null, true, false, 1, "", new stdClass) as $invalid) {
    var_dump(filter_input_array(INPUT_POST, $invalid));
    var_dump(filter_var_array(array(), $invalid));
}
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
