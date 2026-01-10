--TEST--
GH-11476: count_demerits negative-size-param
--EXTENSIONS--
mbstring
--FILE--
<?php
$str = str_repeat('a', 250) . chr(246) . str_repeat('a', 9) . chr(246) . str_repeat('a', 6);
$detectedEncoding = mb_detect_encoding($str, mb_list_encodings(), true);
var_dump($detectedEncoding);
?>
--EXPECT--
string(12) "Windows-1252"
