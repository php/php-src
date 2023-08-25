--TEST--
DOM\HTML5Document serialization of comment
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->appendChild($dom->createComment("foobaré\"<>-&"));
echo $dom->saveHTML();

?>
--EXPECT--
<!--foobaré"<>-&-->
