--TEST--
Test hash_file() function : basic functionality 
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php

/* Prototype  : string hash_hmac  ( string $algo  , string $data  , string $key  [, bool $raw_output  ] )
 * Description: Generate a keyed hash value using the HMAC method
 * Source code: ext/hash/hash.c
 * Alias to functions: 
*/

echo "*** Testing hash_hmac() : basic functionality ***\n";

$content = "This is a sample string used to test the hash_hmac function with various hashing algorithms";
$key = 'secret';

echo "adler32: " . hash_hmac('adler32', $content, $key) . "\n";
echo "crc32: " . hash_hmac('crc32', $content, $key) . "\n";
echo "gost: " . hash_hmac('gost', $content, $key) . "\n";
echo "haval128,3: " . hash_hmac('haval128,3', $content, $key) . "\n";
echo "md2: " . hash_hmac('md2', $content, $key) . "\n";
echo "md4: " . hash_hmac('md4', $content, $key) . "\n";
echo "md5: " . hash_hmac('md5', $content, $key) . "\n";
echo "ripemd128: " . hash_hmac('ripemd128', $content, $key) . "\n";
echo "ripemd160: " . hash_hmac('ripemd160', $content, $key) . "\n";
echo "ripemd256: " . hash_hmac('ripemd256', $content, $key) . "\n";
echo "ripemd320: " . hash_hmac('ripemd320', $content, $key) . "\n";
echo "sha1: " . hash_hmac('sha1', $content, $key) . "\n";
echo "sha256: " . hash_hmac('sha256', $content, $key) . "\n";
echo "sha384: " . hash_hmac('sha384', $content, $key) . "\n";
echo "sha512: " . hash_hmac('sha512', $content, $key) . "\n";
echo "snefru: " . hash_hmac('snefru', $content, $key) . "\n";
echo "tiger192,3: " . hash_hmac('tiger192,3', $content, $key) . "\n";
echo "whirlpool: " . hash_hmac('whirlpool', $content, $key) . "\n";
echo "adler32(raw): " . bin2hex(hash_hmac('adler32', $content, $key, TRUE)) . "\n";
echo "md5(raw): " . bin2hex(hash_hmac('md5', $content, $key, TRUE)) . "\n";
echo "sha256(raw): " . bin2hex(hash_hmac('sha256', $content, $key, TRUE)) . "\n";

?>
===Done===
--EXPECTF--
*** Testing hash_hmac() : basic functionality ***
adler32: 12c803f7
crc32: 96859101
gost: a4a3c80bdf3f8665bf07376a34dc9c1b11af7c813f4928f62e39f0c0dc564dad
haval128,3: 4d1318607f0406bd1b7bd50907772672
md2: 6d111dab563025e4cb5f4425c991fa12
md4: 10cdbfe843000c623f8b8da0d5d20b0b
md5: 2a632783e2812cf23de100d7d6a463ae
ripemd128: 26c2f694a65b1928b668cf55f65529b4
ripemd160: 4b3433ba596ec39692bb7ce760a9ee5fb818113f
ripemd256: 4e4e5ec19322895a727f272dfe68f87bc1af66cc6ce27c6c1360a5ee78a14b30
ripemd320: f10a8ff82e828b92a5ff0a02fc9032bc61352d0d824821fc42f7e09cf5b5f41ee59fd33a730d7469
sha1: 5bfdb62b97e2c987405463e9f7c193139c0e1fd0
sha256: 49bde3496b9510a17d0edd8a4b0ac70148e32a1d51e881ec76faa96534125838
sha384: b781415b856744834e532b9899e1aa0bec5a82cf09a838f0a833470468e2a42648a52428cfd9012385d04de5cd9bd122
sha512: 7de05636b18e2b0ca3427e03f53074af3a48a7b9df226daba4f22324c570638e7d7b26430e214799c9ce0db5ee88dad3292ca0f38bf99b8eaebed59b3a9c140a
snefru: 67af483046f9cf16fe19f9087929ccfc6ad176ade3290b4d33f43e0ddb07e711
tiger192,3: 00a0f884f15a9e5549ed0e40ca0190522d369027e16d5b59
whirlpool: 4a0f1582b21b7aff59bfba7f9c29131c69741b2ce80acdc7d314040f3b768cf5a17e30b74cceb86fbc6b34b1692e0addd5bfd7cfc043d40c0621f1b97e26fa49
adler32(raw): 12c803f7
md5(raw): 2a632783e2812cf23de100d7d6a463ae
sha256(raw): 49bde3496b9510a17d0edd8a4b0ac70148e32a1d51e881ec76faa96534125838
===Done===
