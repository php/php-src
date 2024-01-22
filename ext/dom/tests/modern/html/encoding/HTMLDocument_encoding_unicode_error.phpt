--TEST--
DOM\HTMLDocument loading with unicode codepoints resulting in an error
--EXTENSIONS--
dom
--FILE--
<?php
echo "--- createFromFile ---\n";
DOM\HTMLDocument::createFromFile(__DIR__."/utf16le_error.html");
echo "--- createFromString ---\n";
DOM\HTMLDocument::createFromString(file_get_contents(__DIR__."/utf16le_error.html"));
?>
--EXPECTF--
--- createFromFile ---

Warning: DOM\HTMLDocument::createFromFile(): tokenizer error missing-end-tag-name in %s on line %d

Warning: DOM\HTMLDocument::createFromFile(): tree error unexpected-token in %s on line %d

Warning: DOM\HTMLDocument::createFromFile(): tree error unexpected-token in %s on line %d
--- createFromString ---

Warning: DOM\HTMLDocument::createFromString(): tokenizer error missing-end-tag-name in Entity, line: 7, column: 29 in %s on line %d

Warning: DOM\HTMLDocument::createFromString(): tree error unexpected-token in Entity, line: 7, column: 14-17 in %s on line %d

Warning: DOM\HTMLDocument::createFromString(): tree error unexpected-token in Entity, line: 8, column: 7-10 in %s on line %d
