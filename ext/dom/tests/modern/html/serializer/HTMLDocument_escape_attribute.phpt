--TEST--
Dom\HTMLDocument serialization escape attribute
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString("<p></p>", LIBXML_NOERROR);
$p = $dom->documentElement->firstChild->nextSibling->firstChild;
$p->setAttribute("foo", "<bar>\"'&");
echo $dom->saveHtml();

?>
--EXPECT--
<html><head></head><body><p foo="<bar>&quot;'&amp;"></p></body></html>
