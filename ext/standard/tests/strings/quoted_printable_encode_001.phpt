--TEST--
quoted_printable_encode() tests - 1
--FILE--
<?php

var_dump(quoted_printable_encode(""));
var_dump(quoted_printable_encode("test"));

echo "Done\n";
?>
--EXPECT--
string(0) ""
string(4) "test"
Done
