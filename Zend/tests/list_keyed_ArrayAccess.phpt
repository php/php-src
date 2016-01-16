--TEST--
list() with keys and ArrayAccess
--FILE--
<?php

$antonymObject = new ArrayObject;
$antonymObject["good"] = "bad";
$antonymObject["happy"] = "sad";

list("good" => $good, "happy" => $happy) = $antonymObject;
var_dump($good, $happy);

?>
--EXPECT--
string(3) "bad"
string(3) "sad"
