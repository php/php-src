--TEST--
Bug #52656 (DOMCdataSection does not work with splitText).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$CData = new DOMCdataSection('splithere!');
$CDataSplit = $CData->splitText(5);
echo get_class($CDataSplit), "\n";
var_dump($CDataSplit->data);
?>
--EXPECT--
DOMText
string(5) "here!"
