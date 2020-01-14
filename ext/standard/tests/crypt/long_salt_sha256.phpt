--TEST--
Long salts for SHA-256 in crypt()
--FILE--
<?php

$b = str_repeat("A", 124);
var_dump(crypt("A", "$5$" . $b));
$b = str_repeat("A", 125);
var_dump(crypt("A", "$5$" . $b));
$b = str_repeat("A", 4096);
var_dump(crypt("A", "$5$" . $b));

?>
--EXPECT--
string(63) "$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6"
string(63) "$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6"
string(63) "$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6"
