--TEST--
filter_data() and flags
--FILE--
<?php

var_dump(filter_data("0xff", FL_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("07", FL_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));
var_dump(filter_data("0xff0000", FL_INT, array("flags"=>FILTER_FLAG_ALLOW_HEX)));
var_dump(filter_data("0666", FL_INT, array("flags"=>FILTER_FLAG_ALLOW_OCTAL)));

var_dump(filter_data("6", FL_INT, array("min_range"=>1, "max_range"=>7)));
var_dump(filter_data("6", FL_INT, array("min_range"=>0, "max_range"=>5)));
var_dump(filter_data(-1, FL_INT, array("min_range"=>1, "max_range"=>7)));
var_dump(filter_data(-1, FL_INT, array("min_range"=>-4, "max_range"=>7)));

var_dump(filter_data("", FL_INT, array("min_range"=>-4, "max_range"=>7)));
var_dump(filter_data("", FL_INT, array("min_range"=>2, "max_range"=>7)));
var_dump(filter_data("", FL_INT, array("min_range"=>-5, "max_range"=>-3)));
var_dump(filter_data(345, FL_INT, array("min_range"=>500, "max_range"=>100)));
var_dump(filter_data("0ff", FL_INT));
var_dump(filter_data("010", FL_INT));

echo "Done\n";
?>
--EXPECT--	
int(255)
int(7)
int(16711680)
int(438)
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
