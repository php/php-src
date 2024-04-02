--TEST--
DOM\HTMLDocument::createFromFile() - parser warning 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__."/parser_warning_01.html", LIBXML_NOERROR);
echo $dom->saveHTML(), "\n";

?>
--EXPECT--
<html><head><title>foo</title>
</head><body><datalist id="fruits">
    <option value="Apple">
    </option><option value="Banana">
    </option><option value="Pear">
</option></datalist>
<p>error</p></body></html>
