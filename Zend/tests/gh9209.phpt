--TEST--
GH-9209 (precision 0 -INF)
--FILE--
<?php
ini_set("precision", 0);

var_dump((string)INF);
var_dump((string)-INF);
var_dump((string)NAN);
var_dump((string)-NAN);
var_dump((string)-123);
?>
--EXPECT--
string(3) "INF"
string(4) "-INF"
string(3) "NAN"
string(3) "NAN"
string(4) "-123"
