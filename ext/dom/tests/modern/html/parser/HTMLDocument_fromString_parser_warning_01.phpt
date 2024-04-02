--TEST--
DOM\HTMLDocument::createFromString() - parser warning 01
--EXTENSIONS--
dom
--FILE--
<?php

$html = file_get_contents(__DIR__."/parser_warning_01.html");
$dom = DOM\HTMLDocument::createFromString($html);
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTMLDocument::createFromString(): tokenizer error missing-end-tag-name in Entity, line: 7, column: 11 in %s on line %d

Warning: DOM\HTMLDocument::createFromString(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2-6 in %s on line %d
<html><head><title>foo</title>
</head><body><datalist id="fruits">
    <option value="Apple">
    </option><option value="Banana">
    </option><option value="Pear">
</option></datalist>
<p>error</p></body></html>
