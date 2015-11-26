--TEST--
DOMImplementation::createDocument()
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
$x = new DOMImplementation();
$doc = $x->createDocument(null, 'html');
echo $doc->saveHTML();
?>
--EXPECTF--
<html></html>
