--TEST--
Long salts for MD5 in crypt()
--FILE--
<?php

$b = str_repeat("A", 124);
var_dump(crypt("A", "$1$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$1$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$1$" . $b));

?>
--EXPECT--
string(34) "$1$AAAAAAAA$iPCtA9RBW7JSAIkADiqTK0"
string(34) "$1$AAAAAAAA$iPCtA9RBW7JSAIkADiqTK0"
string(34) "$1$AAAAAAAA$iPCtA9RBW7JSAIkADiqTK0"
