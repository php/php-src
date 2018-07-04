--TEST--
Bug #76285 DOMDocument::formatOutput attribute sometimes ignored
--SKIPIF--
<?php require_once('skipif.inc'); ?>
<?php if (!defined("LIBXML_HTML_NOIMPLIED")) die("skip libxml2 2.7.7 required"); ?>
--FILE--
<?php

$dom = new DOMDocument();
$dom->formatOutput = false;
$html = '<div><div><a>test</a></div><div><a>test2</a></div></div>';
$dom->loadHTML($html, LIBXML_HTML_NOIMPLIED | LIBXML_HTML_NODEFDTD);
$rootNode = $dom->documentElement;
var_dump($dom->saveHTML($rootNode));
var_dump($dom->saveHTML());

?>
--EXPECT--
string(56) "<div><div><a>test</a></div><div><a>test2</a></div></div>"
string(57) "<div><div><a>test</a></div><div><a>test2</a></div></div>
"
