--TEST--
Bug #67474 getElementsByTagNameNS and default namespace
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root xmlns:x="x"><a/><x:a/></root>');
$list = $doc->getElementsByTagNameNS('', 'a');
var_dump($list->length);
$list = $doc->getElementsByTagNameNS(null, 'a');
var_dump($list->length);
?>
--EXPECT--
int(1)
int(1)
