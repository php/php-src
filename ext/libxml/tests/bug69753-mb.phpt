--TEST--
Bug #69753 - libXMLError::file contains invalid URI
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) != 'WIN') die("skip this test is for Windows platforms only");
if (version_compare(LIBXML_DOTTED_VERSION, "2.13.0") < 0) die("skip fails for libxml2 < 2.13.0; https://gitlab.gnome.org/GNOME/libxml2/-/issues/611");
?>
--FILE--
<?php
libxml_use_internal_errors(true);
$doc = new DomDocument();
$doc->load(__DIR__ . DIRECTORY_SEPARATOR . 'bug69753私はガラスを食べられます.xml');
$error = libxml_get_last_error();
var_dump($error->file);
?>
--EXPECTF--
string(%d) "%s\ext\libxml\tests\bug69753私はガラスを食べられます.xml"
