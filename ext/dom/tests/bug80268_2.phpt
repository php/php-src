--TEST--
Bug #80268 (loadHTML() truncates at NUL bytes)
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (LIBXML_VERSION < 20912) die('skip For libxml2 >= 2.9.12 only');
?>
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadHTML("<p>foo\0bar</p>", LIBXML_NOERROR);
$html = $doc->saveHTML();
var_dump(strpos($html, '<p>foo</p>') !== false);

file_put_contents(__DIR__ . '/80268.html', "<p>foo\0bar</p>");
$doc = new DOMDocument;
$doc->loadHTMLFile(__DIR__ . '/80268.html', LIBXML_NOERROR);
$html = $doc->saveHTML();
var_dump(strpos($html, '<p>foo</p>') !== false);
?>
--CLEAN--
<?php
unlink(__DIR__ . '/80268.html');
?>
--EXPECTF--
bool(false)
bool(false)
