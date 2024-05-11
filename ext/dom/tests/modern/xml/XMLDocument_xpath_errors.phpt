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
} catch (Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Warning: Dom\XPath::evaluate(): Invalid expression in %s on line %d
string(35) "Could not evaluate XPath expression"
