--TEST--
Test threads option of password_needs_rehash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_needs_rehash not built with Argon2');
if (PASSWORD_ARGON2_PROVIDER === 'sodium') die('skip libsodium only supports threads=1');
?>
--FILE--
<?php
foreach (['argon2i', 'argon2id'] as $algo) {
	$threads = 2; // allow for valid +/- mutation
	$hash = sprintf(
		'$%s$v=19$m=%d,t=%d,p=2$rmuWsgW7EVRE+0rxKhCFUw$3XoT9PdOvdvimSBt07k/eAC0X6+cZWz7aCfnfepbD2I',
		$algo, PASSWORD_ARGON2_DEFAULT_MEMORY_COST, PASSWORD_ARGON2_DEFAULT_TIME_COST, $threads
	);
	echo sprintf("%s threads + 1: %s\n", $algo, password_needs_rehash($hash, $algo, ['threads' => $threads+1]) ? 'rehash' : 'keep');
	echo sprintf("%s threads - 1: %s\n", $algo, password_needs_rehash($hash, $algo, ['threads' => $threads-1]) ? 'rehash' : 'keep');
}

echo "OK!";
?>
--EXPECT--
argon2i threads + 1: rehash
argon2i threads - 1: rehash
argon2id threads + 1: rehash
argon2id threads - 1: rehash
OK!
