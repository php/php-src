--TEST--
Test Uri\QueryParams::deleteValue() - error - enum unbacked
--FILE--
<?php

enum Flag { case Enabled; }

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $params = $params->deleteValue("x", Flag::Enabled);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->toRfc3986String());

?>
--EXPECT--
TypeError: Uri\QueryParams::deleteValue(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, Flag given
string(3) "x=1"
