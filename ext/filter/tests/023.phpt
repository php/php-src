--TEST--
filter_var() and FILTER_UNSAFE_RAW
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("}\"<p>test para</p>", FILTER_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_var("a[!@#<b>$%^&*()@a@#$%^&*(.<br>com@#$%^&*(", FILTER_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_var("white space here \ \ \" some more", FILTER_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_var("", FILTER_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_var("             123456789000000       <qwertyuiop> ", FILTER_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));

echo "Done\n";
?>
--EXPECT--
string(18) "}"<p>test para</p>"
string(53) "a[!@#<b>$%^&#38;*()@a@#$%^&#38;*(.<br>com@#$%^&#38;*("
string(32) "white space here \ \ " some more"
string(0) ""
string(48) "             123456789000000       <qwertyuiop> "
Done
