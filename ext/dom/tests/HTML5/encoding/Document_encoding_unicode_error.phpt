--TEST--
HTML5Document::loadHTML(File) with unicode codepoints resulting in an error
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOM\HTML5Document();
echo "--- loadHTMLFile ---\n";
$dom->loadHTMLFile(__DIR__."/utf16le_error.html");
echo "--- loadHTML ---\n";
$dom->loadHTML(file_get_contents(__DIR__."/utf16le_error.html"));
?>
--EXPECTF--
--- loadHTMLFile ---

Warning: DOM\HTML5Document::loadHTMLFile(): tokenizer error missing-end-tag-name in %s, line: 7, column: 29 in %s on line %d

Warning: DOM\HTML5Document::loadHTMLFile(): tree error unexpected-token in %s, line: 7, column: 14-17 in %s on line %d

Warning: DOM\HTML5Document::loadHTMLFile(): tree error unexpected-token in %s, line: 8, column: 7-10 in %s on line %d
--- loadHTML ---

Warning: DOM\HTML5Document::loadHTML(): tokenizer error missing-end-tag-name in Entity, line: 7, column: 29 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-token in Entity, line: 7, column: 14-17 in %s on line %d

Warning: DOM\HTML5Document::loadHTML(): tree error unexpected-token in Entity, line: 8, column: 7-10 in %s on line %d
