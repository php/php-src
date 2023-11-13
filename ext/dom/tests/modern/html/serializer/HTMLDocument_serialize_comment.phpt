--TEST--
DOM\HTMLDocument serialization of comment
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom->appendChild($dom->createComment("foobaré\"<>-&"));
echo $dom->saveHTML();

?>
--EXPECT--
<!--foobaré"<>-&-->
