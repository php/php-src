--TEST--
Test Uri\QueryParams::__unserialize() - error - reinitialization
--FILE--
<?php

$params1 = Uri\QueryParams::parseRfc3986("x=1");
$params2 = Uri\QueryParams::parseRfc3986("x=2");

try {
    $params1->__unserialize($params2->__serialize());
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params1->toRfc3986String());

?>
--EXPECT--
Exception: Invalid serialization data for Uri\QueryParams object
string(3) "x=1"
