--TEST--
registerPhpFunctionNS() function - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://PHP.net">hello</a>');

$xpath = new DOMXPath($doc);

try {
    $xpath->registerPhpFunctionNS('http://php.net/xpath', 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS('urn:foo', 'x:a', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("urn:foo", "\0", strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("\0", 'strtolower', strtolower(...));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xpath" because it is reserved by PHP
DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must be a valid callback name
DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must not contain any null bytes
DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not contain any null bytes
