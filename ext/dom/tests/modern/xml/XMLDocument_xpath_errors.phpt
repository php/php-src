--TEST--
Dom\XMLDocument interaction with XPath evaluate - errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');

$xpath = new Dom\XPath($dom);

try {
    var_dump($xpath->evaluate('-'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: Dom\XPath::evaluate(): Invalid expression in %s on line %d
Error: Could not evaluate XPath expression
