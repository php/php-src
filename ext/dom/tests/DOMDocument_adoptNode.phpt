--TEST--
DOMDocument::adoptNode not implemented
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML("<root />");

$dom->adoptNode($dom->documentElement);
?>
--EXPECTF--
Warning: DOMDocument::adoptNode(): Not yet implemented in %s
