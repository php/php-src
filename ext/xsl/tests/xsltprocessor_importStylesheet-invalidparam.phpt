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
try {
    var_dump($xslt->importStylesheet($dummy));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::importStylesheet(): Argument #1 ($stylesheet) must be a valid XML node
