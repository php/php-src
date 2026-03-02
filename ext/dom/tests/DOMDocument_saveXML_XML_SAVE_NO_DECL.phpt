--TEST--
DOMDocument::saveXML(): XML_SAVE_NO_DECL
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (getenv('SKIP_ASAN') && LIBXML_VERSION < 20911) die('xleak leaks under libxml2 versions older than 2.9.11');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root>é</root>');

echo $doc->saveXML(options: 0);
echo $doc->saveXML(options: LIBXML_NOXMLDECL);
// Explicit encoding test, to ensure no encoding declaration.
$doc->encoding = "UTF-8";
echo $doc->saveXML(options: LIBXML_NOXMLDECL);

// Edge case
$doc = new DOMDocument();
var_dump($doc->saveXML(options: LIBXML_NOXMLDECL));
?>
--EXPECT--
<?xml version="1.0"?>
<root>&#xE9;</root>
<root>&#xE9;</root>
<root>é</root>
string(0) ""
