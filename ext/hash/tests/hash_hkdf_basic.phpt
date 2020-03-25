--TEST--
Hash: hash_hkdf() function: basic functionality
--FILE--
<?php

/* Prototype  : string hkdf  ( string $algo  , string $ikm  [, int $length  , string $info = '' , string $salt = ''  ] )
 * Description: HMAC-based Key Derivation Function
 * Source code: ext/hash/hash.c
*/

echo "*** Testing hash_hkdf(): basic functionality ***\n";

$ikm = 'input key material';

echo 'md2: ', bin2hex(hash_hkdf('md2', $ikm)), "\n";
echo 'md4: ', bin2hex(hash_hkdf('md4', $ikm)), "\n";
echo 'md5: ', bin2hex(hash_hkdf('md5', $ikm)), "\n";
echo 'sha1: ', bin2hex(hash_hkdf('sha1', $ikm)), "\n";
echo 'sha224: ', bin2hex(hash_hkdf('sha224', $ikm)), "\n";
echo 'sha256: ', bin2hex(hash_hkdf('sha256', $ikm)), "\n";
echo 'sha384: ', bin2hex(hash_hkdf('sha384', $ikm)), "\n";
echo 'sha512: ', bin2hex(hash_hkdf('sha512', $ikm)), "\n";
echo 'ripemd128: ', bin2hex(hash_hkdf('ripemd128', $ikm)), "\n";
echo 'ripemd160: ', bin2hex(hash_hkdf('ripemd160', $ikm)), "\n";
echo 'ripemd256: ', bin2hex(hash_hkdf('ripemd256', $ikm)), "\n";
echo 'ripemd320: ', bin2hex(hash_hkdf('ripemd320', $ikm)), "\n";
echo 'whirlpool: ', bin2hex(hash_hkdf('whirlpool', $ikm)), "\n";
echo 'tiger128,3: ', bin2hex(hash_hkdf('tiger128,3', $ikm)), "\n";
echo 'tiger160,3: ', bin2hex(hash_hkdf('tiger160,3', $ikm)), "\n";
echo 'tiger192,3: ', bin2hex(hash_hkdf('tiger192,3', $ikm)), "\n";
echo 'tiger128,4: ', bin2hex(hash_hkdf('tiger128,4', $ikm)), "\n";
echo 'tiger160,4: ', bin2hex(hash_hkdf('tiger160,4', $ikm)), "\n";
echo 'tiger192,4: ', bin2hex(hash_hkdf('tiger192,4', $ikm)), "\n";
echo 'haval128,3: ', bin2hex(hash_hkdf('haval128,3', $ikm)), "\n";
echo 'haval160,3: ', bin2hex(hash_hkdf('haval160,3', $ikm)), "\n";
echo 'haval192,3: ', bin2hex(hash_hkdf('haval192,3', $ikm)), "\n";
echo 'haval224,3: ', bin2hex(hash_hkdf('haval224,3', $ikm)), "\n";
echo 'haval256,3: ', bin2hex(hash_hkdf('haval256,3', $ikm)), "\n";
echo 'haval128,4: ', bin2hex(hash_hkdf('haval128,4', $ikm)), "\n";
echo 'haval160,4: ', bin2hex(hash_hkdf('haval160,4', $ikm)), "\n";
echo 'haval192,4: ', bin2hex(hash_hkdf('haval192,4', $ikm)), "\n";
echo 'haval224,4: ', bin2hex(hash_hkdf('haval224,4', $ikm)), "\n";
echo 'haval256,4: ', bin2hex(hash_hkdf('haval256,4', $ikm)), "\n";
echo 'haval128,5: ', bin2hex(hash_hkdf('haval128,5', $ikm)), "\n";
echo 'haval160,5: ', bin2hex(hash_hkdf('haval160,5', $ikm)), "\n";
echo 'haval192,5: ', bin2hex(hash_hkdf('haval192,5', $ikm)), "\n";
echo 'haval224,5: ', bin2hex(hash_hkdf('haval224,5', $ikm)), "\n";
echo 'haval256,5: ', bin2hex(hash_hkdf('haval256,5', $ikm)), "\n";
echo 'snefru: ', bin2hex(hash_hkdf('snefru', $ikm)), "\n";
echo 'snefru256: ', bin2hex(hash_hkdf('snefru256', $ikm)), "\n";
echo 'gost: ', bin2hex(hash_hkdf('gost', $ikm)), "\n";

?>
--EXPECT--
*** Testing hash_hkdf(): basic functionality ***
md2: 87779851d2377dab25da16fd7aadfdf5
md4: 422c6bd8dd2a6baae8abadef618c3ede
md5: 98b16391063ecee006a3ca8ee5776b1e
sha1: a71863230e3782240265126a53e137af6667e988
sha224: 51678ceb17e803505187b2cf6451c30fbc572fda165bb69bbd117c7a
sha256: d8f0bede4b652933c32a92eccf7723f7eeb4701744c81325dc3f0fa9fda24499
sha384: f600680e677bb417a7a22a4da8b167c0d91823a7a5d56a49aeb1838bb2320c05068d15d6d980824fee542a279d310c3a
sha512: fb1b86549e941b81821a89ac6ba7c4f93465077b3f2af94352ebf1d041efcd3c5694469c1ae31bb10db4c1d2ab84f07e4518ba33a3eadd4a149425750285c640
ripemd128: cb6418fc0dc9efaeb7e9654390fa7f14
ripemd160: ba42dbb34f08e9337ace15295f218754a41d6c39
ripemd256: f2e96b292935e2395b59833ed89d928ac1197ff62c8031ebc06a3f5bad19513f
ripemd320: a13a682072525ceb4c4a5fef59096e682096e1096e6e7e238c7bd48a6f6c6a9ba3d7d9fbee6b68c4
whirlpool: 497c717e04d896c3d582742c614435b7d0963b39de12dcf532540d39164b3b85214014620dfdff4a089a06b06aff43c39a3b4d9b806913cf6309de58ff1151f5
tiger128,3: e13c2e7262892c6bd8dfc24121e7cb34
tiger160,3: 48cc5a9f5e5d7029eb0544662222c0ba13822b7b
tiger192,3: 5a665d23b6cbb405668160e58b01aebef74eba979f4bc70b
tiger128,4: 8acf517ecf58cccbd65c1186d71e4116
tiger160,4: cc0e33ee26700a2eb9a994bbb0e6cef29b429441
tiger192,4: 97fa02d42331321fdc05c7f8dbc756d751ca36ce1aee69b0
haval128,3: 2accab8029d42fb15fdbe9d3e2a470ca
haval160,3: 496fd29e7fc8351d2971b96a3733a7b3de000064
haval192,3: 238a731801439b1f195e1a1568ce75251e1dd719d904a8a2
haval224,3: d863e596ff6b2bdba1ed7b313df1c3d177176312e81b47e9290f7566
haval256,3: 96f555fe41255c34fe57b275f1ae40bbb8f07c6a2a6d68c849748fbb393ff443
haval128,4: 9822af229cc59527a72e231a690fad3b
haval160,4: 1bbbc4d632daaf94d5ba167efaa70af5b753effe
haval192,4: dd12a8f8919cbf5632497f0918b30236371dd1b55f71e824
haval224,4: 8af449fb4eb627eb8887507c1279a116ac4325b5806dd22e2f2af410
haval256,4: bd74a6d5fa1ec23a92ce1fd76c36bc8be36f5eddbea821545a91810e1f8d6fc5
haval128,5: 84564f3450a6ccf6041162207dc8acba
haval160,5: b55cd1b3c514457b9e61c51ad22f302f6ec7cca1
haval192,5: d1db7a8e69b327455d530d1ac60f774023b8b4bdd6bbbf92
haval224,5: c5a2576511f1143c6e29f63d82d6e0be8f67d0bea448e27238be5000
haval256,5: 9dbab73d13f1fd3a1b41398fe90ba1f298329681d861b023373c33f1051bd4d3
snefru: 798eac954e5ece38e9acb63b50c1c2ecb799d34356358cec5a80eeeea91c8de9
snefru256: 798eac954e5ece38e9acb63b50c1c2ecb799d34356358cec5a80eeeea91c8de9
gost: 64edd584b87a2dfdd1f2b44ed2db8bd27af8386aafe751c2aebaed32dfa3852e
