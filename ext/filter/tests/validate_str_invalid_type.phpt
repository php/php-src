--TEST--
FILTER_VALIDATE_STR: Invalid input types
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

/**
 * According to the filter_var() manual:
 * https://www.php.net/manual/en/function.filter-var.php
 * - Scalar types (int, float, bool) are automatically converted to strings before filtering.
 * - Non-scalar types (array, object, resource, null) always result in false.
 *
 * This test ensures FILTER_VALIDATE_STR behaves accordingly for various input types.
 */

$options = ['options' => ['min_len' => 2, 'max_len' => 4]];
$handle = fopen("php://memory", "r");
class Dummy { public $x = 1; }

var_dump(
    filter_var(1234, FILTER_VALIDATE_STR, $options),
    filter_var(3.14, FILTER_VALIDATE_STR, $options),
    filter_var(['a', 'b'], FILTER_VALIDATE_STR, $options),
    filter_var(new Dummy(), FILTER_VALIDATE_STR, $options),
    filter_var(NULL, FILTER_VALIDATE_STR, $options),
    filter_var(true, FILTER_VALIDATE_STR, $options),
    filter_var(false, FILTER_VALIDATE_STR, $options),
    filter_var($handle, FILTER_VALIDATE_STR, $options)
);

fclose($handle);

?>
--EXPECT--
string(4) "1234"
string(4) "3.14"
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
