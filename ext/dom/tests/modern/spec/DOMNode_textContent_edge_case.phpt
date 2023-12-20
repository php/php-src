--TEST--
DOMNode::$textContent edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString("foobarbaz");
var_dump($dom->textContent);
$dom->textContent = "x";
var_dump($dom->textContent);

echo $dom->saveHTML(), "\n";

$body = $dom->getElementsByTagName("body")[0];
var_dump($body->textContent);
$body->textContent = "y";
var_dump($body->textContent);

echo $dom->saveHTML(), "\n";

?>
--EXPECT--
string(0) ""
string(0) ""
<html><head></head><body>foobarbaz</body></html>
string(9) "foobarbaz"
string(1) "y"
<html><head></head><body>y</body></html>
