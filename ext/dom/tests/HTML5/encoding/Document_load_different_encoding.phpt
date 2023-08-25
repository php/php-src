--TEST--
DOM\HTML5Document load document with different encoding
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTMLFile(__DIR__ . "/windows1251.html");
var_dump($dom->encoding);
$dom->loadHTML("<p>hé</p>", LIBXML_NOERROR);
var_dump($dom->encoding);
echo $dom->saveHTML();

?>
--EXPECT--
string(12) "windows-1251"
string(5) "UTF-8"
<html><head></head><body><p>hé</p></body></html>
