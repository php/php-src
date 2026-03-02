--TEST--
Test curl_escape and curl_unescape() functions
--EXTENSIONS--
curl
--FILE--
<?php
$str = "http://www.php.net/ ?!";

$a = curl_init();
$escaped = curl_escape($a, $str);
$original = curl_unescape($a, $escaped);
var_dump($escaped, $original);
var_dump(curl_unescape($a, 'a%00b'));
?>
--EXPECTF--
string(36) "http%3A%2F%2Fwww.php.net%2F%20%3F%21"
string(22) "http://www.php.net/ ?!"
string(3) "a%0b"
