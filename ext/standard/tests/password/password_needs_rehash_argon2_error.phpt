--TEST--
Test error operation of password_needs_rehash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_needs_rehash not built with Argon2');
die('skip Argon2 providers each have different behavior!');
?>
--FILE--
<?php
$result = function($hash, $algo, $options) {
	try {
		return password_needs_rehash($hash, $algo, $options) ? 'rehash' : 'keep';
	} catch (ValueError $e) {
		return 'error: '.$e->getMessage();
	}
};

$template = '$%s$v=19$%s$rmuWsgW7EVRE+0rxKhCFUw$3XoT9PdOvdvimSBt07k/eAC0X6+cZWz7aCfnfepbD2I';
// Keep p=1 to not trigger libsodium errors, as it only supports threads=1
$defaults = sprintf('m=%d,t=%d,p=1', PASSWORD_ARGON2_DEFAULT_MEMORY_COST, PASSWORD_ARGON2_DEFAULT_TIME_COST);
$modified = sprintf('m=%d,t=%d,p=1', PASSWORD_ARGON2_DEFAULT_MEMORY_COST-1, PASSWORD_ARGON2_DEFAULT_TIME_COST-1);

foreach (['argon2i', 'argon2id'] as $algo) {
	$hash = sprintf($template, $algo, $defaults);
	echo sprintf("%s defaults with invalid memory_cost: %s\n", $algo, $result($hash, $algo, ['memory_cost' => 7]));
	echo sprintf("%s defaults with invalid time_cost: %s\n", $algo, $result($hash, $algo, ['time_cost' => 0]));
	echo sprintf("%s defaults with invalid threads: %s\n", $algo, $result($hash, $algo, ['threads' => 0]));
	$hash = sprintf($template, $algo, $modified);
	echo sprintf("%s non-defaults with invalid memory_cost: %s\n", $algo, $result($hash, $algo, ['memory_cost' => 7]));
	echo sprintf("%s non-defaults with invalid time_cost: %s\n", $algo, $result($hash, $algo, ['time_cost' => 0]));
	echo sprintf("%s non-defaults with invalid threads: %s\n", $algo, $result($hash, $algo, ['threads' => 0]));
}

/* libargon2 output:
argon2i defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2i defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2i defaults with invalid threads: error: A thread value other than 1 is not supported by this implementation
argon2i non-defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2i non-defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2i non-defaults with invalid threads: error: A thread value other than 1 is not supported by this implementation
argon2id defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2id defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2id defaults with invalid threads: error: A thread value other than 1 is not supported by this implementation
argon2id non-defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2id non-defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2id non-defaults with invalid threads: error: A thread value other than 1 is not supported by this implementation
*/

/* openssl output:
argon2i defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2i defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2i defaults with invalid threads: error: Invalid number of threads
argon2i non-defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2i non-defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2i non-defaults with invalid threads: error: Invalid number of threads
argon2id defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2id defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2id defaults with invalid threads: error: Invalid number of threads
argon2id non-defaults with invalid memory_cost: error: Memory cost is outside of allowed memory range
argon2id non-defaults with invalid time_cost: error: Time cost is outside of allowed time range
argon2id non-defaults with invalid threads: error: Invalid number of threads
*/

// Output based on logic that *if* password_needs_rehash() doesn't trigger exceptions, it would fallback
// to a default for each invalid value.
// Initial libargon2 implementation blindly returned true on any diff though. If you're not reading this
// during a PR review and chasing a breaking change, look at the commit history for this file. It's the first one.
// With each Argon2 provider acting differently, this is effectively undefined behavior yet.
echo "OK!";
?>
--EXPECT--
argon2i defaults with invalid memory_cost: keep
argon2i defaults with invalid time_cost: keep
argon2i defaults with invalid threads: keep
argon2i non-defaults with invalid memory_cost: rehash
argon2i non-defaults with invalid time_cost: rehash
argon2i non-defaults with invalid threads: rehash
argon2id defaults with invalid memory_cost: keep
argon2id defaults with invalid time_cost: keep
argon2id defaults with invalid threads: keep
argon2id non-defaults with invalid memory_cost: rehash
argon2id non-defaults with invalid time_cost: rehash
argon2id non-defaults with invalid threads: rehash
OK!
