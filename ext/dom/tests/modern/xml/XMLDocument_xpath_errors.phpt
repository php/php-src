--TEST--
DOM\XMLDocument interaction with XPath evaluate - errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root/>');

$xpath = new DOM\XPath($dom);

try {
    var_dump($xpath->evaluate('-'));
} catch (Error $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
Warning: DOM\XPath::evaluate(): Invalid expression in %s on line %d
string(35) "Could not evaluate XPath expression"
