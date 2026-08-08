--TEST--
php:function() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadHTML('<a href="https://php.net">hello</a>');
$xpath = new DOMXpath($doc);
$xpath->registerNamespace("php", "http://php.net/xpath");
$xpath->registerPHPFunctions();
try {
    $xpath->query("//a[php:function(3)]");
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $xpath->query("//a[php:function()]");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Handler name must be a string
Error: Function name must be passed as the first argument
