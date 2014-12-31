--TEST--
Bug #53965 (<xsl:include> cannot find files with relative paths when loaded with "file://")
--SKIPIF--
<?php
if (!extension_loaded('xsl')) die("skip Extension XSL is required\n");
?>
--FILE--
<?php

$base = 'file://' . dirname(__FILE__) . DIRECTORY_SEPARATOR . '53965';

$xml = new DOMDocument();
$xml->load($base . DIRECTORY_SEPARATOR . 'collection.xml');

$xsl = new DOMDocument();
$xsl->load($base . DIRECTORY_SEPARATOR . 'collection.xsl');

$proc = new XSLTProcessor;
$proc->importStyleSheet($xsl);

echo $proc->transformToXML($xml);
?>
--EXPECTF--
Hey! Welcome to Nicolas Eliaszewicz's sweet CD collection! 
  
 <h1>Fight for your mind</h1><h2>by Ben Harper - 1995</h2><hr>
 <h1>Electric Ladyland</h1><h2>by Jimi Hendrix - 1997</h2><hr>
