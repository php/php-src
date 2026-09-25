--TEST--
GH-23897 (Assertion failure in php_dom_xpath_callback_dispatch() after failed registerPhpFunctions())
--EXTENSIONS--
dom
--FILE--
<?php
$dom = new DOMDocument();
$dom->loadXML('<root/>');

$xpath = new DOMXPath($dom);
$xpath->registerNamespace('php', 'http://php.net/xpath');
try {
    $xpath->registerPhpFunctions('testPhpFunction');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($xpath->evaluate('php:function("testPhpFunction")'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
TypeError: DOMXPath::registerPhpFunctions(): Argument #1 ($restrict) must be a callable, function "testPhpFunction" not found or invalid function name
Error: No callbacks were registered
