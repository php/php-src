--TEST--
Calling XSLTProcessor::transformToDoc() without stylesheet
--EXTENSIONS--
xsl
--FILE--
<?php

$doc = new DOMDocument('1.0', 'utf-8');

$xsl = new XSLTProcessor;
try {
    $xsl->transformToDoc($doc);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::transformToDoc() can only be called after a stylesheet has been imported
