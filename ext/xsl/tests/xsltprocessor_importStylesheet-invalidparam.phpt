--TEST--
XSLTProcessor::importStylesheet() - Test with invalid stylesheet
--EXTENSIONS--
xsl
--FILE--
<?php

$xslt = new XSLTProcessor();
$dummy = new stdClass();
try {
    var_dump($xslt->importStylesheet($dummy));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::importStylesheet(): Argument #1 ($stylesheet) must be a valid XML node
