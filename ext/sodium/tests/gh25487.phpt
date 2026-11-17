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
sodium_memzero($a->s);
var_dump(bin2hex($a->s));

$a->s = str_repeat('x', 4);
sodium_memzero($a->s);
var_dump(bin2hex($a->s));

$a->state = sodium_crypto_generichash_init();
var_dump(strlen(sodium_crypto_generichash_final($a->state)));
var_dump($a->state === str_repeat("\0", 384));

sodium_memzero($a->nullable);
var_dump($a->nullable);

$a->nullable = sodium_crypto_generichash_init();
var_dump(strlen(sodium_crypto_generichash_final($a->nullable)));
var_dump($a->nullable);
?>
--EXPECT--
string(12) "000000000000"
string(8) "00000000"
int(32)
bool(true)
NULL
int(32)
NULL
