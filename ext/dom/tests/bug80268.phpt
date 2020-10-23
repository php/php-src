--TEST--
Bug #80268 (loadHTML() truncates at NUL bytes)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadHTML("<p>foo\0bar</p>");
$html = $doc->saveHTML();
var_dump(strpos($html, '<p>foo</p>') !== false);

file_put_contents(__DIR__ . '/80268.html', "<p>foo\0bar</p>");
$doc = new DOMDocument;
$doc->loadHTMLFile(__DIR__ . '/80268.html');
$html = $doc->saveHTML();
var_dump(strpos($html, '<p>foo</p>') !== false);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/80268.html');
?>
--EXPECT--
bool(true)
bool(true)
