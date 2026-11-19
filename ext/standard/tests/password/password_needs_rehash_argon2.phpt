--TEST--
Test normal operation of password_needs_rehash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_needs_rehash not built with Argon2');
?>
--FILE--
<?php
$result = function($hash, $algo, $options) {
	try {
		return password_needs_rehash($hash, $algo, $options) ? 'rehash' : 'keep';
	} catch (ValueError $e) {
		return 'error ' . $e->getMessage();
	}
};

$options  = sprintf('m=%d,t=%d,p=%d', PASSWORD_ARGON2_DEFAULT_MEMORY_COST, PASSWORD_ARGON2_DEFAULT_TIME_COST, PASSWORD_ARGON2_DEFAULT_THREADS);
$template = '$%s'.sprintf('$v=19$%s$rmuWsgW7EVRE+0rxKhCFUw$3XoT9PdOvdvimSBt07k/eAC0X6+cZWz7aCfnfepbD2I', $options);

foreach (['argon2i', 'argon2id'] as $algo) {
	$hash = sprintf($template, $algo);
	$old  = password_get_info($hash);

	echo sprintf("%s unchanged: %s\n", $old['algo'], $result($hash, $old['algo'], $old['options']));

	$new_algo = ($algo === 'argon2i') ? 'argon2id' : 'argon2i';
	echo sprintf("%s to %s: %s\n", $algo, $new_algo, $result($hash, $new_algo, $old['options']));

	// Don't test threads here as the libsodium implementation errors out; separate test for that
	foreach (['memory_cost', 'time_cost'] as $o) {
		$new_options     = $old['options'];
		$new_options[$o] = $old['options'][$o] + 1;
		echo sprintf("%s %s + 1: %s\n", $old['algo'], $o, $result($hash, $old['algo'], $new_options));
		$new_options[$o] = $old['options'][$o] - 1;
		echo sprintf("%s %s - 1: %s\n", $old['algo'], $o, $result($hash, $old['algo'], $new_options));
	}
}

echo "OK!";
?>
--EXPECT--
argon2i unchanged: keep
argon2i to argon2id: rehash
argon2i memory_cost + 1: rehash
argon2i memory_cost - 1: rehash
argon2i time_cost + 1: rehash
argon2i time_cost - 1: rehash
argon2id unchanged: keep
argon2id to argon2i: rehash
argon2id memory_cost + 1: rehash
argon2id memory_cost - 1: rehash
argon2id time_cost + 1: rehash
argon2id time_cost - 1: rehash
OK!
