--TEST--
quoted_printable_encode() tests - 1
--FILE--
<?php

var_dump(quoted_printable_encode(""));
var_dump(quoted_printable_encode("test"));

var_dump(quoted_printable_encode(1));
var_dump(quoted_printable_encode(false));

echo "Done\n";
?>
--EXPECTF--
string(0) ""
string(4) "test"
string(1) "1"

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
string(0) ""
Done
