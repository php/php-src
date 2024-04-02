--TEST--
DOM\HTMLDocument::createFromFile() - parser warning libxml_get_last_error()
--EXTENSIONS--
dom
--FILE--
<?php

libxml_use_internal_errors(true);

$html = '<>x</> <!doctype html>';
$dom = DOM\HTMLDocument::createFromFile(__DIR__."/parser_warning_01.html");

var_dump(libxml_get_last_error());

?>
--EXPECTF--
object(LibXMLError)#2 (6) {
  ["level"]=>
  int(2)
  ["code"]=>
  int(1)
  ["column"]=>
  int(2)
  ["message"]=>
  string(%d) "tree error unexpected-token-in-initial-mode in %sparser_warning_01.html, line: 1, column: 2-6"
  ["file"]=>
  string(%d) "%sparser_warning_01.html"
  ["line"]=>
  int(1)
}
