--TEST--
filter_var() and FILTER_SANITIZE_STRING
--EXTENSIONS--
filter
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
--EXPECTF--
Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(0) ""

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(0) ""

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(12) "!@#$%^&*()'""

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(24) "!@#$%^&#38;*()&#39;&#34;"

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(11) "`1234567890"

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(5) "`123`"

Deprecated: Constant FILTER_SANITIZE_STRING is deprecated in %s on line %d
string(1) "."
Done
