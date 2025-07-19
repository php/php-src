--TEST--
FILTER_VALIDATE_STR: Emoji string length
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$options1 = ['options' => ['min_range' => 2, 'max_range' => 2]];
$options2 = ['options' => ['max_range' => 2, 'default' => 'error']];
$options3 = ['options' => ['min_range' => 0]];

var_dump(
    filter_var('ab', filter_id('validate_str'), $options1),
    filter_var('🐘🐘', FILTER_VALIDATE_STR, $options1),
    filter_var('🐘', FILTER_VALIDATE_STR, $options1),
    filter_var('🐘🐘🐘', FILTER_VALIDATE_STR, $options1),
    filter_var('🐘🐘🐘', FILTER_VALIDATE_STR, $options2),
    filter_var('', FILTER_VALIDATE_STR, $options3),
);
?>
--EXPECT--
string(2) "ab"
string(8) "🐘🐘"
bool(false)
bool(false)
string(5) "error"
string(0) ""
