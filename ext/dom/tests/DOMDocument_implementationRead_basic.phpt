--TEST--
DOMDocument::DOMImplementation - basic test for DomDocument::DOMImplementation
--CREDITS--
Lev Radin <prokurator@gmail.com>
# TestFest 2009 NYPHP
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$doc = new DOMDocument;
$doc->load(__DIR__."/book.xml");

var_dump($doc->implementation);


?>
--EXPECTF--
object(DOMImplementation)#%d (0) {
}
