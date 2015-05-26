--TEST--
Bug #68946 - simplexml_load_string() result depends on formatting of xml string
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) die("skip SimpleXML not available");
?>
--FILE--
<?php
$sxe = simplexml_load_string("<?BrightCloud version=bcap/1.1?><bcap><seqnum>1</seqnum></bcap>");
var_dump($sxe == false);
$sxe = simplexml_load_string("<?BrightCloud version=bcap/1.1?><bcap> <seqnum>1</seqnum></bcap>");
var_dump($sxe == false);
?>
--EXPECT--
bool(false)
bool(false)
