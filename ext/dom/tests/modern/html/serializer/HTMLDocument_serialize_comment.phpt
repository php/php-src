--TEST--
Dom\HTMLDocument serialization of comment
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$dom->appendChild($dom->createComment("foobaré\"<>-&"));
echo $dom->saveHtml();

?>
--EXPECT--
<!--foobaré"<>-&-->
