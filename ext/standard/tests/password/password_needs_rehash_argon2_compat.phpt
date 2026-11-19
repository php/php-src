--TEST--
Test interoperability of password_needs_rehash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_needs_rehash not built with Argon2');
// libsodium has inverted conditions for what to do with differing salt/digest lengths - it keeps shorter ones, and wants to rehash longer ones
// openssl defers to keeping any differing lengths
if (PASSWORD_ARGON2_PROVIDER !== 'standard') die('skip Argon2 not provided by libargon2');
?>
--FILE--
<?php
$result = function($hash, $algo, $options) {
	try {
		return password_needs_rehash($hash, $algo, $options) ? 'rehash' : 'keep';
	} catch (ValueError $e) {
		return 'error';
	}
};

$template = '$%s$v=19$%s$%s$%s';
$defaults = sprintf('m=%d,t=%d,p=%d', PASSWORD_ARGON2_DEFAULT_MEMORY_COST, PASSWORD_ARGON2_DEFAULT_TIME_COST, PASSWORD_ARGON2_DEFAULT_THREADS);
// Don't change threads, as libsodium only supports p=1 and triggers errors
$modified = sprintf('m=%d,t=%d,p=%d', PASSWORD_ARGON2_DEFAULT_MEMORY_COST-1, PASSWORD_ARGON2_DEFAULT_TIME_COST+1, PASSWORD_ARGON2_DEFAULT_THREADS);
// Note that only AQgw are always valid last B64 characters
$salt     = str_repeat('A', 22);
$salt_s   = str_repeat('g', 11); // shortest valid
$salt_l   = str_repeat('w', 64); // longest valid
$digest   = str_repeat('Q', 43);
$digest_s = str_repeat('g', 16); // shortest valid
$digest_l = str_repeat('w', 86); // longest valid

$hashes = [
	// Pre-fill with this one edge case, the rest will be generated for both algos
	'argon2i implicit v1.0, default params' => sprintf('$argon2i$%s$%s$%s', $defaults, $salt, $digest),
	'argon2i explicit v1.0, default params' => sprintf('$argon2i$v=16$%s$%s$%s', $defaults, $salt, $digest),
	'argon2i implicit v1.0, modified params' => sprintf('$argon2i$%s$%s$%s', $defaults, $salt, $digest),
	'argon2i explicit v1.0, modified params' => sprintf('$argon2i$v=16$%s$%s$%s', $defaults, $salt, $digest),
];

foreach (['argon2i', 'argon2id'] as $algo) {
	// Should be considered configuration changes, but larger sizes shouldn't trigger a rehash to avoid downgrades
	$hashes[$algo.' shorter salt, default params'] = sprintf($template, $algo, $defaults, $salt_s, $digest);
	$hashes[$algo.' longer salt, default params'] = sprintf($template, $algo, $defaults, $salt_l, $digest);
	$hashes[$algo.' shorter digest, default params'] = sprintf($template, $algo, $defaults, $salt, $digest_s);
	$hashes[$algo.' longer digest, default params'] = sprintf($template, $algo, $defaults, $salt, $digest_l);
	// All should trigger rehash due to modified options
	$hashes[$algo.' shorter salt, modified params'] = sprintf($template, $algo, $modified, $salt_s, $digest);
	$hashes[$algo.' longer salt, modified params'] = sprintf($template, $algo, $modified, $salt_l, $digest);
	$hashes[$algo.' shorter digest, modified params'] = sprintf($template, $algo, $modified, $salt, $digest_s);
	$hashes[$algo.' longer digest, modified params'] = sprintf($template, $algo, $modified, $salt, $digest_l);
}

foreach ($hashes as $scenario => $hash) {
	// Don't use password_get_info() to avoid cascading bug effects with unusual formats
	$algo = explode('$', $hash, 3)[1];
	echo sprintf("%s: %s\n", $scenario, $result($hash, $algo, []));
}

echo "OK!";
?>
--EXPECT--
argon2i implicit v1.0, default params: rehash
argon2i explicit v1.0, default params: rehash
argon2i implicit v1.0, modified params: rehash
argon2i explicit v1.0, modified params: rehash
argon2i shorter salt, default params: rehash
argon2i longer salt, default params: keep
argon2i shorter digest, default params: rehash
argon2i longer digest, default params: keep
argon2i shorter salt, modified params: rehash
argon2i longer salt, modified params: rehash
argon2i shorter digest, modified params: rehash
argon2i longer digest, modified params: rehash
argon2id shorter salt, default params: rehash
argon2id longer salt, default params: keep
argon2id shorter digest, default params: rehash
argon2id longer digest, default params: keep
argon2id shorter salt, modified params: rehash
argon2id longer salt, modified params: rehash
argon2id shorter digest, modified params: rehash
argon2id longer digest, modified params: rehash
OK!
