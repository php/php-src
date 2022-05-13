--TEST--
Bug #69753 - libXMLError::file contains invalid URI
--XFAIL--
Awaiting upstream fix: https://bugzilla.gnome.org/show_bug.cgi?id=750365
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");
?>
--FILE--
<?php
libxml_use_internal_errors(true);
$doc = new DomDocument();
$doc->load(__DIR__ . DIRECTORY_SEPARATOR . 'bug69753.xml');
$error = libxml_get_last_error();
var_dump($error->file);
?>
--EXPECTF--
string(%d) "file:///%s/ext/libxml/tests/bug69753.xml"
