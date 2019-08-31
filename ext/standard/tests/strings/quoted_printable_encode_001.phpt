--TEST--
quoted_printable_encode() tests - 1
--FILE--
<?php

var_dump(quoted_printable_encode(""));
var_dump(quoted_printable_encode("test"));

var_dump(quoted_printable_encode(1));
var_dump(quoted_printable_encode(NULL));
var_dump(quoted_printable_encode(false));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(4) "test"
string(1) "1"
string(0) ""
string(0) ""
Done
