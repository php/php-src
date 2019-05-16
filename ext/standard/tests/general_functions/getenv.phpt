--TEST--
getenv() basic tests
--ENV--
FOO=bar
--FILE--
<?php

var_dump(getenv("FOO"));
var_dump(getenv()["FOO"]);

echo "Done\n";
?>
--EXPECT--
string(3) "bar"
string(3) "bar"
Done
