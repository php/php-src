--TEST--
Test error operation of password_get_info() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_get_info not built with Argon2');
if (PASSWORD_ARGON2_PROVIDER !== 'standard') die('skip Argon2 not provided by libargon2');
?>
--FILE--
<?php
// openssl and libsodium don't reject any of these
$reject = include(__DIR__.'/argon2_malformed_hashes.inc');
echo sprintf("Verifying that %d invalid hashes are rejected ...", count($reject));
$ok = true;
foreach ($reject as $k => $test) {
	if (password_get_info($test)['algoName'] !== 'unknown') {
		$ok = false;
		echo sprintf("\n%s (%s) was NOT REJECTED!", $test, $k);
		continue;
	}
}
echo ($ok) ? " OK\n" : "\n";
echo "OK!"
?>
--EXPECTF--
Verifying that %d invalid hashes are rejected ... OK
OK!
