--TEST--
Bug #47848 (importNode doesn't preserve attribute namespaces)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$aDOM = new DOMDocument();
$aDOM->appendChild($aDOM->createElementNS('http://friend2friend.net/','f2f:a'));

$fromdom = new DOMDocument();
$fromdom->loadXML('<data xmlns:ai="http://altruists.org" ai:attr="namespaced" />');

$attr= $fromdom->firstChild->attributes->item(0);

$att = $aDOM->importNode($attr);

$aDOM->documentElement->appendChild($aDOM->importNode($attr, true));

echo $aDOM->saveXML();

?>
--EXPECT--
<?xml version="1.0"?>
<f2f:a xmlns:f2f="http://friend2friend.net/" xmlns:ai="http://altruists.org" ai:attr="namespaced"/>
