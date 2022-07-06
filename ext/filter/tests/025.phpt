--TEST--
filter_var() and FILTER_SANITIZE_STRING
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("", FILTER_SANITIZE_STRING));
var_dump(filter_var("<>", FILTER_SANITIZE_STRING));
var_dump(filter_var("<>!@#$%^&*()'\"", FILTER_SANITIZE_STRING, FILTER_FLAG_NO_ENCODE_QUOTES));
var_dump(filter_var("<>!@#$%^&*()'\"", FILTER_SANITIZE_STRING, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_var("<>`1234567890", FILTER_SANITIZE_STRING));
var_dump(filter_var("`123`", FILTER_SANITIZE_STRING));
var_dump(filter_var(".", FILTER_SANITIZE_STRING));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(0) ""
string(12) "!@#$%^&*()'""
string(24) "!@#$%^&#38;*()&#39;&#34;"
string(11) "`1234567890"
string(5) "`123`"
string(1) "."
Done
