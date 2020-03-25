--TEST--
Test the basics to function XSLTProcessor::hasExsltSupport().
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--SKIPIF--
<?php extension_loaded('xsl') or die('skip xsl extension is not available'); ?>
--FILE--
<?php
$proc = new XSLTProcessor();
var_dump($proc->hasExsltSupport());
?>
--EXPECT--
bool(true)
