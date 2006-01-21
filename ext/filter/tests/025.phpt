--TEST--
filter_data() and FILTER_SANITIZE_STRING
--FILE--
<?php

var_dump(filter_data("", FILTER_SANITIZE_STRING));
var_dump(filter_data("<>", FILTER_SANITIZE_STRING));
var_dump(filter_data("<>!@#$%^&*()'\"", FILTER_SANITIZE_STRING, FILTER_FLAG_NO_ENCODE_QUOTES));
var_dump(filter_data("<>!@#$%^&*()'\"", FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_data("<>`1234567890", FILTER_SANITIZE_STRING));
var_dump(filter_data("`123`", FILTER_SANITIZE_STRING));
var_dump(filter_data(".", FILTER_SANITIZE_STRING));

echo "Done\n";
?>
--EXPECT--	
NULL
NULL
string(12) "!@#$%^&*()'""
string(32) "!@#$%^&#38;*()&#38;#39;&#38;#34;"
string(11) "`1234567890"
string(5) "`123`"
string(1) "."
Done
