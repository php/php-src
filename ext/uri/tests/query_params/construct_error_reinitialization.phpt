--TEST--
Test Uri\QueryParams::__construct() - error - reinitialization
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $params->__construct();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->toRfc3986String());

?>
--EXPECT--
Error: Cannot modify readonly object of class Uri\QueryParams
string(3) "x=1"
