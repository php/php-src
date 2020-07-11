--TEST--
XSLTProcessor::importStylesheet() - Test with invalid stylesheet
--SKIPIF--
<?php
if (!extension_loaded('xsl')) {
    exit('Skip - XSL extension not loaded');
}
?>
--FILE--
<?php

$xslt = new XSLTProcessor();
$dummy = new stdClass();
var_dump($xslt->importStylesheet($dummy));

?>
--EXPECTF--
Warning: Invalid Document in %s on line %d
bool(false) 
