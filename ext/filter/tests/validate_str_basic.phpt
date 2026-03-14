--TEST--
FILTER_VALIDATE_STR: Emoji string length
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
$options1 = ['options' => ['min_len' => 2, 'max_len' => 2]];
$options2 = ['options' => ['max_len' => 2, 'default' => 'error']];
$options3 = ['options' => ['min_len' => 0]];

var_dump(
    filter_var('ab', filter_id('validate_strlen'), $options1),
    filter_var('🐘🐘', FILTER_VALIDATE_STRLEN, $options1),
    filter_var('🐘', FILTER_VALIDATE_STRLEN, $options1),
    filter_var('🐘🐘🐘', FILTER_VALIDATE_STRLEN, $options1),
    filter_var('🐘🐘🐘', FILTER_VALIDATE_STRLEN, $options2),
    filter_var('', FILTER_VALIDATE_STRLEN, $options3),
);
?>
--EXPECT--
string(2) "ab"
string(8) "🐘🐘"
bool(false)
bool(false)
string(5) "error"
string(0) ""
