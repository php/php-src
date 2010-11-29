--TEST--
Bug #52656 (DOMCdataSection does not work with splitText).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$CData = new DOMCdataSection('splithere!');
$CDataSplit = $CData->splitText(5);
var_dump($CDataSplit, $CDataSplit->data);
?>
--EXPECT--
object(DOMText)#2 (0) {
}
string(5) "here!"
