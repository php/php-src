--TEST--
registerPHPFunctionNS() function - error cases
--EXTENSIONS--
xsl
--FILE--
<?php

require __DIR__ . '/xpath_callables.inc';

try {
    createProcessor([])->registerPhpFunctionNS('http://php.net/xsl', 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS('urn:foo', 'x:a', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS("urn:foo", "\0", strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    createProcessor([])->registerPhpFunctionNS("\0", 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
XSLTProcessor::registerPHPFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xsl" because it is reserved by PHP
XSLTProcessor::registerPHPFunctionNS(): Argument #2 ($name) must be a valid callback name
XSLTProcessor::registerPHPFunctionNS(): Argument #2 ($name) must not contain any null bytes
XSLTProcessor::registerPHPFunctionNS(): Argument #1 ($namespaceURI) must not contain any null bytes
