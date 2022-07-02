--TEST--
Bug #39621 (str_replace() is not binary safe on strings with equal length)
--FILE--
<?php

$search =  "qxxx\0qqqqqqqq";
$subject = "qxxx\0xxxxxxxx";
$replace = "any text";

$result = str_replace ( $search, $replace, $subject );

var_dump($result);

$search =  "QXXX\0qqqqqqqq";
$subject = "qxxx\0xxxxxxxx";
$replace = "any text";

$result = str_ireplace ( $search, $replace, $subject );

var_dump($result);

$search =  "qxxx\0xxxxxxxx";
$subject = "qxxx\0xxxxxxxx";
$replace = "any text";

$result = str_replace ( $search, $replace, $subject );

var_dump($result);

$search =  "qXxx\0xXxXxXxx";
$subject = "qxXx\0xxxxxxxx";
$replace = "any text";

$result = str_ireplace ( $search, $replace, $subject );

var_dump($result);

echo "Done\n";
?>
--EXPECTF--
string(13) "qxxx%0xxxxxxxx"
string(13) "qxxx%0xxxxxxxx"
string(8) "any text"
string(8) "any text"
Done
