--TEST--
GH-25487 (sodium_memzero() sets a typed property to null without a TypeError)
--CREDITS--
lacatoire
--EXTENSIONS--
sodium
--FILE--
<?php
class A {
    public string $s = 'secret';
    public string $state;
    public ?string $nullable = 'secret';
}

$a = new A;
try {
    sodium_memzero($a->s);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($a->s);

$a->state = sodium_crypto_generichash_init();
try {
    sodium_crypto_generichash_final($a->state);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(strlen($a->state));

sodium_memzero($a->nullable);
var_dump($a->nullable);

$a->nullable = sodium_crypto_generichash_init();
var_dump(strlen(sodium_crypto_generichash_final($a->nullable)));
var_dump($a->nullable);
?>
--EXPECT--
Cannot assign null to reference held by property A::$s of type string
string(6) "secret"
Cannot assign null to reference held by property A::$state of type string
int(384)
NULL
int(32)
NULL
