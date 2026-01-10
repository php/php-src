--TEST--
XSLTProcessor::$maxTemplateVars errors
--EXTENSIONS--
xsl
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php

$processor = new XSLTProcessor;
$oldValue = $processor->maxTemplateVars;

try {
    $processor->maxTemplateVars = -1;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateVars === $oldValue);

try {
    $processor->maxTemplateVars = -32.1;
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateVars === $oldValue);

try {
    $processor->maxTemplateVars = "-1";
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

var_dump($processor->maxTemplateVars === $oldValue);

?>
--EXPECT--
XSLTProcessor::$maxTemplateVars must be greater than or equal to 0
bool(true)
XSLTProcessor::$maxTemplateVars must be greater than or equal to 0
bool(true)
XSLTProcessor::$maxTemplateVars must be greater than or equal to 0
bool(true)
