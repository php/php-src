--TEST--
Test error operation of password_hash() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php
$stringify = function(array $options): string {
	$output = '';
	foreach ($options as $k => $v) {
		$output .= ",{$k}={$v}";
	}
	return ltrim($output, ",");
};

$options = [
	["memory_cost" => 0],
	["time_cost" => 0],
	["threads" => 0],
	["threads" => 256], // openssl accepts it and somehow computes super fast?! blind hard-coded 1?
	["memory_cost" => 7],
	["memory_cost" => ($threads = 2)*8-1, "threads" => $threads] // openssl errors, but does attempt hashing and the error is different
];

foreach ([PASSWORD_ARGON2I, PASSWORD_ARGON2ID] as $algo) {
	foreach ($options as $o) {
		try {
			$result = password_hash('test', $algo, $o);
		} catch (ValueError $exception) {
			$result = 'error: '.$exception->getMessage();
		}

		echo sprintf("%s %s: %s\n", $algo, $stringify($o), $result);
	}
}

echo "OK!"
// Note: libsodium always triggers an error about not supporting threads!=1
?>
--EXPECTF--
argon2i memory_cost=0: error: Memory cost is outside of allowed memory range
argon2i time_cost=0: error: Time cost is outside of allowed time range
argon2i threads=0: error: %s
argon2i threads=256: error: %s
argon2i memory_cost=7: error: Memory cost is outside of allowed memory range
argon2i memory_cost=15,threads=2: error: %s
argon2id memory_cost=0: error: Memory cost is outside of allowed memory range
argon2id time_cost=0: error: Time cost is outside of allowed time range
argon2id threads=0: error: %s
argon2id threads=256: error: %s
argon2id memory_cost=7: error: Memory cost is outside of allowed memory range
argon2id memory_cost=15,threads=2: error: %s
OK!
