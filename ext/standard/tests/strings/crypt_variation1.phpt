--TEST--
crypt() function - long salt
--FILE--
<?php

$b = str_repeat("A", 124);
echo crypt("A", "$5$" . $b)."\n";
$b = str_repeat("A", 125);
echo crypt("A", "$5$" . $b)."\n";
$b = str_repeat("A", 4096);
echo crypt("A", "$5$" . $b)."\n";

?>
--EXPECT--
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
