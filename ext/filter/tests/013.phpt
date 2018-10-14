--TEST--
filter_var() and flags
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php
var_dump(filter_var("  234", FILTER_VALIDATE_INT));
var_dump(filter_var("234    ", FILTER_VALIDATE_INT));
var_dump(filter_var("  234  ", FILTER_VALIDATE_INT));
var_dump(filter_var("0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_var("0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_var("08", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_var("00", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_var("000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));

var_dump(filter_var("-0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("-0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("-0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("-0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("-07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_var("-0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_var("-0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));

var_dump(filter_var("6", FILTER_VALIDATE_INT, array("options" => array("min_range"=>1, "max_range"=>7))));
var_dump(filter_var("6", FILTER_VALIDATE_INT, array("options" => array("min_range"=>0, "max_range"=>5))));
var_dump(filter_var(-1, FILTER_VALIDATE_INT, array("options" => array("min_range"=>1, "max_range"=>7))));
var_dump(filter_var(-1, FILTER_VALIDATE_INT, array("options" => array("min_range"=>-4, "max_range"=>7))));

var_dump(filter_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>-4, "max_range"=>7))));
var_dump(filter_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>2, "max_range"=>7))));
var_dump(filter_var("", FILTER_VALIDATE_INT, array("options" => array("min_range"=>-5, "max_range"=>-3))));
var_dump(filter_var(345, FILTER_VALIDATE_INT, array("options" => array("min_range"=>500, "max_range"=>100))));
var_dump(filter_var("0ff", FILTER_VALIDATE_INT));
var_dump(filter_var("010", FILTER_VALIDATE_INT));

echo "Done\n";
?>
--EXPECT--
int(234)
int(234)
int(234)
int(255)
int(255)
int(255)
int(255)
int(7)
int(16711680)
int(438)
bool(false)
int(0)
int(0)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
int(6)
bool(false)
bool(false)
int(-1)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Done
