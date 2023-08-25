--TEST--
DOM\HTML5Document serialization escape text 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$dom->loadHTML("<p></p>", LIBXML_NOERROR);
$p = $dom->documentElement->firstChild->nextSibling->firstChild;
$p->textContent = "this is <some> &text! \"\"";
echo $dom->saveHTML();

?>
--EXPECT--
<html><head></head><body><p>this is &lt;some&gt; &amp;text! ""</p></body></html>
