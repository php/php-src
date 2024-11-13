--TEST--
Dom\Text:splitText() edge case
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\HTMLDocument::createEmpty();
$text = $dom->createTextNode('Hello World!');
var_dump($text->splitText(5)->wholeText);
try {
    var_dump($text->splitText(-1));
} catch (ValueError $e) {
  echo $e->getMessage(), "\n";
}
try {
    var_dump($text->splitText(200));
} catch (DOMException $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(7) " World!"
Dom\Text::splitText(): Argument #1 ($offset) must be greater than or equal to 0
Index Size Error
