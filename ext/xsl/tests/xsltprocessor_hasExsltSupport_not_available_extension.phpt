--TEST--
Test the basics to function XSLTProcessor::hasExsltSupport() when the xsl extension os not available.
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php (!extension_loaded('xsl')) or die('skip xsl extension is available'); ?>
--FILE--
<?php
$proc = new XSLTProcessor();
var_dump($proc->hasExsltSupport());
?>
--EXPECTF--
Fatal error: Class 'XSLTProcessor' not found in %s on line %i