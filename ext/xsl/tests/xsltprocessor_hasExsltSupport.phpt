--TEST--
Test the basics to function XSLTProcessor::hasExsltSupport().
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
--EXTENSIONS--
xsl
--FILE--
<?php
$proc = new XSLTProcessor();
var_dump($proc->hasExsltSupport());
?>
--EXPECT--
bool(true)
