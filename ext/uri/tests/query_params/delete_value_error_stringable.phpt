--TEST--
Test Uri\QueryParams::deleteValue() - error - stringable with strict types
--FILE--
<?php

declare(strict_types=1);

class C implements Stringable {
    public function __toString(): string {
        throw new Exception("Must not be called");
    }
}

$value = new C();

$params = Uri\QueryParams::parseRfc3986("x=1");

try {
    $params = $params->deleteValue("x", $value);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($params->toRfc3986String());

?>
--EXPECT--
TypeError: Uri\QueryParams::deleteValue(): Argument #2 ($value) must be of type BackedEnum|string|int|float|null, C given
string(3) "x=1"
