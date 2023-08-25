--TEST--
DOM\HTML5Document::loadHTML() - parser warning 01
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();
$html = file_get_contents(__DIR__."/parser_warning_01.html");
$dom->loadHTML($html);
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTML5Document::loadHTML(): tokenizer error missing-end-tag-name in Entity, line: 7, column: 11 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 2-6 in %s on line %d
<html><head><title>foo</title>
</head><body><datalist id="fruits">
    <option value="Apple">
    </option><option value="Banana">
    </option><option value="Pear">
</option></datalist>
<p>error</p></body></html>
