--TEST--
Test hash_hmac_file() function : basic functionality 
--SKIPIF--
<?php extension_loaded('hash') or die('skip: hash extension not loaded.'); ?>
--FILE--
<?php


/* Prototype  : string hash_hmac_file ( string algo, string filename, string key [, bool raw_output] )
 * Description: Generate a keyed hash value using the HMAC method and the contents of a given file
 * Source code: ext/hash/hash.c
 * Alias to functions: 
*/

echo "*** Testing hash_hmac_file() : basic functionality ***\n";

$file = dirname(__FILE__) . "hash_hmac_file.txt";
/* Creating a temporary file file */
if (($fp = fopen( $file, "w+")) == FALSE) {
	echo "Cannot create file ($file)";
    exit;
}	

/* Writing into file */ 
$content = "This is a sample string used to test the hash_hmac_file function with various hashing algorithms";
if (is_writable($file)) {
  if (fwrite($fp, $content) === FALSE) {
    echo "Cannot write to file ($file)";
    exit;
  }
}

// close the files 
fclose($fp);

$key = 'secret';


echo "adler32: " . hash_hmac_file('adler32', $file, $key) . "\n";
echo "crc32: " . hash_hmac_file('crc32', $file, $key) . "\n";
echo "gost: " . hash_hmac_file('gost', $file, $key) . "\n";
echo "haval128,3: " . hash_hmac_file('haval128,3', $file, $key) . "\n";
echo "md2: " . hash_hmac_file('md2', $file, $key) . "\n";
echo "md4: " . hash_hmac_file('md4', $file, $key) . "\n";
echo "md5: " . hash_hmac_file('md5', $file, $key) . "\n";
echo "ripemd128: " . hash_hmac_file('ripemd128', $file, $key) . "\n";
echo "ripemd160: " . hash_hmac_file('ripemd160', $file, $key) . "\n";
echo "ripemd256: " . hash_hmac_file('ripemd256', $file, $key) . "\n";
echo "ripemd320: " . hash_hmac_file('ripemd320', $file, $key) . "\n";
echo "sha1: " . hash_hmac_file('sha1', $file, $key) . "\n";
echo "sha256: " . hash_hmac_file('sha256', $file, $key) . "\n";
echo "sha384: " . hash_hmac_file('sha384', $file, $key) . "\n";
echo "sha512: " . hash_hmac_file('sha512', $file, $key) . "\n";
echo "snefru: " . hash_hmac_file('snefru', $file, $key) . "\n";
echo "tiger192,3: " . hash_hmac_file('tiger192,3', $file, $key) . "\n";
echo "whirlpool: " . hash_hmac_file('whirlpool', $file, $key) . "\n";

echo "adler32(raw): " . bin2hex(hash_hmac_file('adler32', $file, $key, TRUE)) . "\n";
echo "md5(raw): " . bin2hex(hash_hmac_file('md5', $file, $key, TRUE)). "\n";
echo "sha256(raw): " . bin2hex(hash_hmac_file('sha256', $file, $key, TRUE)). "\n";

echo "Error cases:\n";
hash_hmac_file();
hash_hmac_file('foo', $file);
hash_hmac_file('foo', $file, $key, TRUE, 10);

unlink($file);

?>
===Done===
--EXPECTF--
*** Testing hash_hmac_file() : basic functionality ***
adler32: 0f8c02f9
crc32: f2a60b9c
gost: 94c39a40d5db852a8dc3d24e37eebf2d53e3d711457c59cd02b614f792a9d918
haval128,3: e8fcff647f1a675acb429130fb94a17e
md2: a685475e600314bb549ab4f33c3b27cb
md4: cbc6bff781f48f57378d3effa27553e4
md5: 8bddf39dd1c566c27acc7fa85ec36acf
ripemd128: 03269b76bf61d508c50f038cbe9ba691
ripemd160: 94652211292268d97eb63344a3a05d3009f9d2d3
ripemd256: b6ab414cc1630e1e474fefa41976d252f38ca7cf401552774e71736165e512e7
ripemd320: 71271a649265740eed4b9931417f979fd81eba6288f4e08ff2997bc3dd6858da054d53a9f1fffe8c
sha1: 7f338d17b72371091abd28f451bc8d1f3a9eb3b6
sha256: 9135286ca4c84dec711e4b831f6cd39e672e5ff93d011321274eb76733cc1e40
sha384: 364fdc45a4c742763366ab5d3d1c17c24057e6c3b641607a36d969f00c88da25b19c8b88c8632411e3a0a02397f88aca
sha512: d460aabdf0353655059ed0d408efa91f19c4cda46acc2a4e0adf4764b06951c899fbb2ed41519db78b58ff7be17b1b2910aebe674a56861b232143571b35c83f
snefru: 7b79787e1c1d926b6cc98327f05c5d04ba6227ab51c1398661861196016ef34c
tiger192,3: ca89badf843ba68e3fae5832635aa848a72a4bc11676edd4
whirlpool: 37a0fbb90547690d5e5e11c046f6654ffdb7bab15e16d9d79c7d85765cc4bdcbfd9df8db7a3ce9558f3f244fead00ca29cf05297f75596555195a0683f15d69f
adler32(raw): 0f8c02f9
md5(raw): 8bddf39dd1c566c27acc7fa85ec36acf
sha256(raw): 9135286ca4c84dec711e4b831f6cd39e672e5ff93d011321274eb76733cc1e40
Error cases:

Warning: hash_hmac_file() expects at least 3 parameters, 0 given in %s on line %d

Warning: hash_hmac_file() expects at least 3 parameters, 2 given in %s on line %d

Warning: hash_hmac_file() expects at most 4 parameters, 5 given in %s on line %d
===Done===
