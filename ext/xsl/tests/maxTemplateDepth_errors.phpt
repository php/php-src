--TEST--
XSLTProcessor::$maxTemplateDepth errors
--EXTENSIONS--
xsl
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php

$processor = new XSLTProcessor;
$oldValue = $processor->maxTemplateDepth;

try {
    $processor->maxTemplateDepth = -1;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateDepth === $oldValue);

try {
    $processor->maxTemplateDepth = -32.1;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateDepth === $oldValue);

try {
    $processor->maxTemplateDepth = "-1";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateDepth === $oldValue);

?>
--EXPECT--
XSLTProcessor::$maxTemplateDepth must be greater than or equal to 0
bool(true)
XSLTProcessor::$maxTemplateDepth must be greater than or equal to 0
bool(true)
XSLTProcessor::$maxTemplateDepth must be greater than or equal to 0
bool(true)
