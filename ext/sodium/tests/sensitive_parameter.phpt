--TEST--
Test that the secret scalar parameter of sodium_crypto_scalarmult*() is marked sensitive.
--EXTENSIONS--
sodium
--FILE--
<?php
foreach ([
    'sodium_crypto_scalarmult',
    'sodium_crypto_scalarmult_base',
    'sodium_crypto_scalarmult_ristretto255',
    'sodium_crypto_scalarmult_ristretto255_base',
] as $function) {
    $parameter = (new ReflectionFunction($function))->getParameters()[0];
    echo $function, '($', $parameter->getName(), '): ';
    var_dump(array_map(fn (ReflectionAttribute $a) => $a->getName(), $parameter->getAttributes()));
}
?>
--EXPECT--
sodium_crypto_scalarmult($n): array(1) {
  [0]=>
  string(18) "SensitiveParameter"
}
sodium_crypto_scalarmult_base($secret_key): array(1) {
  [0]=>
  string(18) "SensitiveParameter"
}
sodium_crypto_scalarmult_ristretto255($n): array(1) {
  [0]=>
  string(18) "SensitiveParameter"
}
sodium_crypto_scalarmult_ristretto255_base($n): array(1) {
  [0]=>
  string(18) "SensitiveParameter"
}
