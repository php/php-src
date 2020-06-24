--TEST--
Hash: hash_hkdf() function: edge cases
--FILE--
<?php

echo "*** Testing hash_hkdf(): edge cases ***\n";

$ikm = 'input key material';

echo 'Length < digestSize: ', bin2hex(hash_hkdf('md5', $ikm, 7)), "\n";
echo 'Length % digestSize != 0: ', bin2hex(hash_hkdf('md5', $ikm, 17)), "\n";
echo 'Algo name case-sensitivity: ', (bin2hex(hash_hkdf('Md5', $ikm, 7)) === '98b16391063ece' ? 'true' : 'false'), "\n";
echo "Non-crypto algo name case-sensitivity:\n";

try {
    var_dump(hash_hkdf('jOaAt', $ikm));
}
catch (\Error $e) {
    echo '[Error] ' . $e->getMessage() . "\n";
}

?>
--EXPECT--
*** Testing hash_hkdf(): edge cases ***
Length < digestSize: 98b16391063ece
Length % digestSize != 0: 98b16391063ecee006a3ca8ee5776b1e5f
Algo name case-sensitivity: true
Non-crypto algo name case-sensitivity:
[Error] hash_hkdf(): Argument #1 ($algo) must be a valid cryptographic hashing algorithm
