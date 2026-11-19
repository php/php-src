--TEST--
Hash: GOST R 34.11-94 algorithm
--FILE--
<?php
echo hash('gost', ''), "\n";
echo hash('gost', 'The quick brown fox jumps over the lazy cog'), "\n";
echo hash('gost', str_repeat('a', 31)), "\n";
echo hash('gost', str_repeat('a', 32)), "\n";
echo hash('gost', str_repeat('a', 33)), "\n";

// Commercial variant by CryptoPro with different S-boxes.
// No official standards body has published test vectors for it.
echo hash('gost-crypto', ''), "\n";
echo hash('gost-crypto', 'The quick brown fox jumps over the lazy cog'), "\n";
echo hash('gost-crypto', str_repeat('a', 31)), "\n";
echo hash('gost-crypto', str_repeat('a', 32)), "\n";
echo hash('gost-crypto', str_repeat('a', 33)), "\n";

// Original Russian publication only has the same 2 vectors.
echo "*** RFC 5831 ***\n";
echo hash("gost", "This is message, length=32 bytes"), "\n";
echo hash("gost", "Suppose the original message has length = 50 bytes"), "\n";
?>
--EXPECT--
ce85b99cc46752fffee35cab9a7b0278abb4c2d2055cff685af4912c49490f8d
a3ebc4daaab78b0be131dab5737a7f67e602670d543521319150d2e14eeec445
03840d6348763f11e28e7b1ecc4da0cdf7f898fa555b928ef684c6c5b8f46d9f
fd1b746d9397e78edd311baef391450434271e02816caa37680d6d7381c79d4e
715e59cdc8ebde9fdf0fe2a2e811b3bf7f48209a01505e467d2cd2aa2bbb5ecf
981e5f3ca30c841487830f84fb433e13ac1101569b9c13584ac483234cd656c0
a93124f5bf2c6d83c3bbf722bc55569310245ca5957541f4dbd7dfaf8137e6f2
8978e06b0ecf54ea81ec51ca4e02bcb4eb390b3f04cb5f65ee8de195ffae591b
e121e3740ae94ca6d289e6d653ff31695783efff3dd960417a1098a0130fa720
d3e8f22d9762a148ddfc84a6043d97a608604dae7c05baee72b55f559d03dd74
*** RFC 5831 ***
b1c466d37519b82e8319819ff32595e047a28cb6f83eff1c6916a815a637fffa
471aba57a60a770d3a76130635c1fbea4ef14de51f78b4ae57dd893b62f55208
