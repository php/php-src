--TEST--
DOMImplementation::createDocument()
--EXTENSIONS--
dom
--FILE--
<?php
declare(strict_types=1);
$x = new DOMImplementation();
$doc = $x->createDocument(null, 'html');
echo $doc->saveHTML();
?>
--EXPECT--
<html></html>
