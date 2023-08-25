--TEST--
DOM\HTML5Document::loadHTML() - parser warning via internal error
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

libxml_use_internal_errors(true);

$html = '<>x</> <!doctype html>';
$dom->loadHTML($html);
foreach (libxml_get_errors() as $error) {
    var_dump($error->message, $error->line, $error->column);
}

?>
--EXPECT--
string(81) "tokenizer error invalid-first-character-of-tag-name in Entity, line: 1, column: 2"
int(1)
int(2)
string(66) "tokenizer error missing-end-tag-name in Entity, line: 1, column: 6"
int(1)
int(6)
string(75) "tree error unexpected-token-in-initial-mode in Entity, line: 1, column: 1-7"
int(1)
int(1)
string(71) "tree error doctype-token-in-body-mode in Entity, line: 1, column: 10-16"
int(1)
int(10)
