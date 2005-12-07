--TEST--
input_get()
--FILE--
<?php

var_dump(input_get(INPUT_GET, "test"));
var_dump(input_get(INPUT_POST, "test"));
var_dump(input_get(INPUT_COOKIE, ""));

echo "Done\n";
?>
--EXPECT--	
bool(false)
bool(false)
bool(false)
Done
