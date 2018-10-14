--TEST--
crypt() function - long salt
--SKIPIF--
<?php
if (!function_exists('crypt')) {
	die("SKIP crypt() is not available");
}
?>
--FILE--
<?php

$b = str_repeat("A", 124);
echo crypt("A", "$5$" . $b)."\n";
$b = str_repeat("A", 125);
echo crypt("A", "$5$" . $b)."\n";
$b = str_repeat("A", 4096);
echo crypt("A", "$5$" . $b)."\n";

?>
--EXPECTF--
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
$5$AAAAAAAAAAAAAAAA$frotiiztWZiwcncxnY5tWG9Ida2WOZEximjLXCleQu6
