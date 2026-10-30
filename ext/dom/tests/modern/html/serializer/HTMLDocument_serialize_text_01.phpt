--TEST--
Dom\HTMLDocument serialization escape text 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString("<p></p>", LIBXML_NOERROR);
$p = $dom->documentElement->firstChild->nextSibling->firstChild;
$p->textContent = "this is <some> &text! \"\"";
echo $dom->saveHtml();

?>
--EXPECT--
<html><head></head><body><p>this is &lt;some&gt; &amp;text! ""</p></body></html>
