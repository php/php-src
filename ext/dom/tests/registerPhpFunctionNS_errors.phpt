--TEST--
registerPhpFunctionNS() function - error cases
--EXTENSIONS--
dom
--FILE--
<?php

class TrampolineClass {
    public function __call($name, $args) {
    }
}

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://PHP.net">hello</a>');

$xpath = new DOMXPath($doc);

try {
    $xpath->registerPhpFunctionNS('http://php.net/xpath', 'strtolower', strtolower(...));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS('http://php.net/xpath', 'test', [new TrampolineClass, 'test']);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS('urn:foo', '$$$', [new TrampolineClass, 'test']);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS('urn:foo', 'x:a', strtolower(...));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("urn:foo", "\0", strtolower(...));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $xpath->registerPhpFunctionNS("\0", 'strtolower', strtolower(...));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xpath" because it is reserved by PHP
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not be "http://php.net/xpath" because it is reserved by PHP
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must be a valid callback name
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must be a valid callback name
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #2 ($name) must not contain any null bytes
ValueError: DOMXPath::registerPhpFunctionNS(): Argument #1 ($namespaceURI) must not contain any null bytes
