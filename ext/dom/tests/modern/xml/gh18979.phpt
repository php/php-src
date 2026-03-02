--TEST--
GH-18979 (DOM\XMLDocument::createComment() triggers undefined behavior with null byte)
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createEmpty();
$container = $dom->createElement("container");
$container->append($dom->createComment("\0"));
var_dump($container->innerHTML);
?>
--EXPECT--
string(7) "<!---->"
