--TEST--
Check for libsodium generichash
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$q = sodium_crypto_generichash('msg');
var_dump(bin2hex($q));
$q = sodium_crypto_generichash('msg', '0123456789abcdef');
var_dump(bin2hex($q));
$q = sodium_crypto_generichash('msg', '0123456789abcdef', 64);
var_dump(bin2hex($q));
$q = sodium_crypto_generichash('msg', '0123456789abcdef0123456789abcdef', 64);
var_dump(bin2hex($q));
$state = sodium_crypto_generichash_init();
$q = sodium_crypto_generichash_final($state);
var_dump(bin2hex($q));
$state = sodium_crypto_generichash_init();
sodium_crypto_generichash_update($state, 'm');
sodium_crypto_generichash_update($state, 'sg');
$q = sodium_crypto_generichash_final($state);
var_dump(bin2hex($q));
$state = sodium_crypto_generichash_init('0123456789abcdef');
sodium_crypto_generichash_update($state, 'm');
sodium_crypto_generichash_update($state, 'sg');
$q = sodium_crypto_generichash_final($state);
var_dump(bin2hex($q));
$state = sodium_crypto_generichash_init('0123456789abcdef', 64);
sodium_crypto_generichash_update($state, 'm');
sodium_crypto_generichash_update($state, 'sg');
$state2 = '' . $state;
$q = sodium_crypto_generichash_final($state, 64);
var_dump(bin2hex($q));
sodium_crypto_generichash_update($state2, '2');
$q = sodium_crypto_generichash_final($state2, 64);
$exp = bin2hex($q);
var_dump($exp);
$act = bin2hex(
    sodium_crypto_generichash('msg2', '0123456789abcdef', 64)
);
var_dump($act);
var_dump($exp === $act);
try {
    $hash = sodium_crypto_generichash('test', '', 128);
} catch (SodiumException $ex) {
    var_dump(true);
}
?>
--EXPECT--
string(64) "96a7ed8861db0abc006f473f9e64687875f3d9df8e723adae9f53a02b2aec378"
string(64) "ba03e32a94ece425a77b350f029e0a3d37e6383158aa7cefa2b1b9470a7fcb7a"
string(128) "8ccd640462e7380010c5722d7f3c2354781d1360430197ff233509c27353fd2597c8d689bfe769467056a0655b3faba6af4e4ade248558f7c53538c4d5b94806"
string(128) "30f0e5f1e3beb7e0340976ac05a94043cce082d870e28e03c906e8fe9a88786271c6ba141eee2885e7444a870fac498cc78a13b0c53aefaec01bf38ebfe73b3f"
string(64) "0e5751c026e543b2e8ab2eb06099daa1d1e5df47778f7787faab45cdf12fe3a8"
string(64) "96a7ed8861db0abc006f473f9e64687875f3d9df8e723adae9f53a02b2aec378"
string(64) "ba03e32a94ece425a77b350f029e0a3d37e6383158aa7cefa2b1b9470a7fcb7a"
string(128) "8ccd640462e7380010c5722d7f3c2354781d1360430197ff233509c27353fd2597c8d689bfe769467056a0655b3faba6af4e4ade248558f7c53538c4d5b94806"
string(128) "9ef702f51114c9dc2cc7521746e8beebe0a3ca9bb29ec729e16682ca982e7f69ff70235a46659a9a6c28f92fbd990288301b9a6b5517f1f2ba6518074af19a5a"
string(128) "9ef702f51114c9dc2cc7521746e8beebe0a3ca9bb29ec729e16682ca982e7f69ff70235a46659a9a6c28f92fbd990288301b9a6b5517f1f2ba6518074af19a5a"
bool(true)
bool(true)
