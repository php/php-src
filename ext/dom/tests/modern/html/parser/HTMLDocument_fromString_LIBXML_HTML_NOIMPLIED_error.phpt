--TEST--
DOM\HTMLDocument::createFromString() with LIBXML_HTML_NOIMPLIED - tree error should not happen
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString("<div>foo</div>", LIBXML_HTML_NOIMPLIED);
echo $dom->saveHTML();

?>
--EXPECT--
<div>foo</div>
