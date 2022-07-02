--TEST--
Check for libsodium scalarmult ristretto255
--EXTENSIONS--
sodium
--SKIPIF--
<?php
if (!defined('SODIUM_CRYPTO_CORE_RISTRETTO255_HASHBYTES')) print "skip libsodium without Ristretto255";
?>
--FILE--
<?php
$b = sodium_hex2bin("e2f2ae0a6abc4e71a884a961c500515f58e30b6aa582dd8db6a65945e08d2d76");
$n = str_repeat("\0", SODIUM_CRYPTO_SCALARMULT_RISTRETTO255_SCALARBYTES);

try {
    $p = sodium_crypto_scalarmult_ristretto255_base($n);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    $p2 = sodium_crypto_scalarmult_ristretto255($n, $b);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}

for ($i = 1; $i < 16; $i++) {
    sodium_increment($n);
    $p = sodium_crypto_scalarmult_ristretto255_base($n);
    $p2 = sodium_crypto_scalarmult_ristretto255($n, $b);
    var_dump(sodium_bin2hex($p));
    assert($p === $p2);
}

try {
    sodium_crypto_scalarmult(substr($n, 1), $p);
} catch (SodiumException $ex) {
    echo $ex->getMessage(), "\n";
}

?>
--EXPECT--
sodium_crypto_scalarmult_ristretto255_base(): Argument #1 ($n) must not be zero
Result is identity element
string(64) "e2f2ae0a6abc4e71a884a961c500515f58e30b6aa582dd8db6a65945e08d2d76"
string(64) "6a493210f7499cd17fecb510ae0cea23a110e8d5b901f8acadd3095c73a3b919"
string(64) "94741f5d5d52755ece4f23f044ee27d5d1ea1e2bd196b462166b16152a9d0259"
string(64) "da80862773358b466ffadfe0b3293ab3d9fd53c5ea6c955358f568322daf6a57"
string(64) "e882b131016b52c1d3337080187cf768423efccbb517bb495ab812c4160ff44e"
string(64) "f64746d3c92b13050ed8d80236a7f0007c3b3f962f5ba793d19a601ebb1df403"
string(64) "44f53520926ec81fbd5a387845beb7df85a96a24ece18738bdcfa6a7822a176d"
string(64) "903293d8f2287ebe10e2374dc1a53e0bc887e592699f02d077d5263cdd55601c"
string(64) "02622ace8f7303a31cafc63f8fc48fdc16e1c8c8d234b2f0d6685282a9076031"
string(64) "20706fd788b2720a1ed2a5dad4952b01f413bcf0e7564de8cdc816689e2db95f"
string(64) "bce83f8ba5dd2fa572864c24ba1810f9522bc6004afe95877ac73241cafdab42"
string(64) "e4549ee16b9aa03099ca208c67adafcafa4c3f3e4e5303de6026e3ca8ff84460"
string(64) "aa52e000df2e16f55fb1032fc33bc42742dad6bd5a8fc0be0167436c5948501f"
string(64) "46376b80f409b29dc2b5f6f0c52591990896e5716f41477cd30085ab7f10301e"
string(64) "e0c418f7c8d9c4cdd7395b93ea124f3ad99021bb681dfc3302a9d99a2e53e64e"
sodium_crypto_scalarmult(): Argument #1 ($n) must be SODIUM_CRYPTO_SCALARMULT_SCALARBYTES bytes long
