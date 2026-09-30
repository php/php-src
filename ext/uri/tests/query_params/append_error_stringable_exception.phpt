--TEST--
Test Uri\QueryParams::append() - error - Stringable exception is propagated
--FILE--
<?php

class Value {
    public function __toString(): string {
        throw new Exception("Conversion failed");
    }
}

$params = Uri\QueryParams::parseRfc3986("a=1");
$value = new Value();
try {
    $params = $params->append("a", $value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($value instanceof Value);
var_dump($params->toRfc3986String());

?>
--EXPECT--
Exception: Conversion failed
bool(true)
string(3) "a=1"
