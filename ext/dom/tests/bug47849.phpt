--TEST--
Bug #47849 (Non-deep import loses the namespace).
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php 

$aDOM= new DOMDocument();
$aDOM->appendChild($aDOM->createElementNS('urn::root','r:root'));

$fromdom= new DOMDocument();
$fromdom->loadXML('<data xmlns="urn::data">aaa</data>');

$data= $fromdom->documentElement;
$aDOM->documentElement->appendChild($aDOM->importNode($data));

echo $aDOM->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<r:root xmlns:r="urn::root"><data xmlns="urn::data"/></r:root>
