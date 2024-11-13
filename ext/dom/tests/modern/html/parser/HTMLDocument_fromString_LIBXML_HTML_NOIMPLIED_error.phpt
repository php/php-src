--TEST--
Dom\HTMLDocument::createFromString() with LIBXML_HTML_NOIMPLIED - tree error should not happen
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString("<div>foo</div>", LIBXML_HTML_NOIMPLIED);
echo $dom->saveHtml();

?>
--EXPECT--
<div>foo</div>
