--TEST--
filter_data() and FS_UNSAFE_RAW
--GET--

--FILE--
<?php

var_dump(filter_data("}\"<p>test para</p>", FS_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_data("a[!@#<b>$%^&*()@a@#$%^&*(.<br>com@#$%^&*(", FS_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_data("white space here \ \ \" some more", FS_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_data("", FS_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
var_dump(filter_data("             123456789000000       <qwertyuiop> ", FS_UNSAFE_RAW, FILTER_FLAG_ENCODE_AMP));
	
echo "Done\n";
?>
--EXPECT--	
string(18) "}"<p>test para</p>"
string(53) "a[!@#<b>$%^&#38;*()@a@#$%^&#38;*(.<br>com@#$%^&#38;*("
string(32) "white space here \ \ " some more"
string(0) ""
string(48) "             123456789000000       <qwertyuiop> "
Done
