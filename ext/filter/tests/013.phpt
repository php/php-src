--TEST--
filter_data() and flags
--FILE--
<?php

var_dump(filter_data("0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("08", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("00", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));

var_dump(filter_data("-0xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("-0Xff", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("-0xFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("-0XFF", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("-07", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("-0xff0000", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("-0666", FILTER_VALIDATE_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));

var_dump(filter_data("6", FILTER_VALIDATE_INT, array("min_range"=>1, "max_range"=>7)));
var_dump(filter_data("6", FILTER_VALIDATE_INT, array("min_range"=>0, "max_range"=>5)));
var_dump(filter_data(-1, FILTER_VALIDATE_INT, array("min_range"=>1, "max_range"=>7)));
var_dump(filter_data(-1, FILTER_VALIDATE_INT, array("min_range"=>-4, "max_range"=>7)));

var_dump(filter_data("", FILTER_VALIDATE_INT, array("min_range"=>-4, "max_range"=>7)));
var_dump(filter_data("", FILTER_VALIDATE_INT, array("min_range"=>2, "max_range"=>7)));
var_dump(filter_data("", FILTER_VALIDATE_INT, array("min_range"=>-5, "max_range"=>-3)));
var_dump(filter_data(345, FILTER_VALIDATE_INT, array("min_range"=>500, "max_range"=>100)));
var_dump(filter_data("0ff", FILTER_VALIDATE_INT));
var_dump(filter_data("010", FILTER_VALIDATE_INT));

echo "Done\n";
?>
--EXPECT--	
int(255)
int(255)
int(255)
int(255)
int(7)
int(16711680)
int(438)
NULL
int(0)
int(0)
int(-255)
int(-255)
int(-255)
int(-255)
int(-7)
int(-16711680)
int(-438)
int(6)
NULL
NULL
int(-1)
int(0)
NULL
NULL
NULL
NULL
NULL
Done
