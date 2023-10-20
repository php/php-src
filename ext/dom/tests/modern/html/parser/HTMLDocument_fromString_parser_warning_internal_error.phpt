--TEST--
DOM\HTMLDocument::createFromString() - parser warning via internal error
--EXTENSIONS--
dom
--FILE--
<?php

libxml_use_internal_errors(true);

$html = '<>x</> <!doctype html>';
$dom = DOM\HTMLDocument::createFromString($html);
foreach (libxml_get_errors() as $error) {
    var_dump($error->message, $error->line, $error->column);
}

var_dump(libxml_get_last_error());

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
object(LibXMLError)#4 (6) {
  ["level"]=>
  int(2)
  ["code"]=>
  int(1)
  ["column"]=>
  int(10)
  ["message"]=>
  string(71) "tree error doctype-token-in-body-mode in Entity, line: 1, column: 10-16"
  ["file"]=>
  string(0) ""
  ["line"]=>
  int(1)
}
