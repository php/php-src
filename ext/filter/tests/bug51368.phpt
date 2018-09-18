--TEST--
FR #51368 (php_filter_float does not allow custom thousand separators)
--SKIPIF--
<?php
if (!extension_loaded('filter')) die('skip filter extension not available');
?>
--FILE--
<?php
$options = ['flags' => FILTER_FLAG_ALLOW_THOUSAND, 'options' => ['thousand' => ' ']];
var_dump(
    filter_var('1 000', FILTER_VALIDATE_FLOAT, $options),
    filter_var('1 234.567', FILTER_VALIDATE_FLOAT, $options)
);
$options = ['flags' => FILTER_FLAG_ALLOW_THOUSAND, 'options' => ['thousand' => '']];
var_dump(filter_var('12345', FILTER_VALIDATE_FLOAT, $options));
?>
===DONE===
--EXPECTF--
float(1000)
float(1234.567)

Warning: filter_var(): thousand separator must be at least one char in %s on line %d
bool(false)
===DONE===
