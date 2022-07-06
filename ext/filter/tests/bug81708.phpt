--TEST--
Bug #81708 (UAF due to php_filter_float() failing for ints)
--SKIPIF--
<?php
if (!extension_loaded("filter")) die("skip filter extension not available");
?>
--INI--
opcache.enable_cli=0
--FILE--
<?php
$input = "+" . str_repeat("1", 2); // avoid string interning
filter_var(
    $input,
    FILTER_VALIDATE_FLOAT,
    ["options" => ['min_range' => -1, 'max_range' => 1]]
);
var_dump($input);
?>
--EXPECT--
string(3) "+11"
