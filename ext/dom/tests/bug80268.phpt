--TEST--
Bug #80268 (loadHTML() truncates at NUL bytes)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadHTML("<p>foobar</p>");
$html = $doc->saveHTML();
var_dump(strpos($html, 'foobar') !== false);
?>
--EXPECT--
bool(true)
