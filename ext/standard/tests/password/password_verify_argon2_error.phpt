--TEST--
Test error operation of password_verify() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_verify not built with Argon2');
if (PASSWORD_ARGON2_PROVIDER === 'openssl') die('skip Argon2 provided by openssl');
if (PASSWORD_ARGON2_PROVIDER === 'sodium') die('skip Argon2 provided by libsodium');
?>
--FILE--
<?php
$reject = include(__DIR__.'/argon2_malformed_hashes.inc');

// These can freeze up the entire machine, so be *very* careful.
// libargon2 ("standard" provider) implementation is already tested to be safe.
// Extra guard here so if the skip is removed for other providers, it's still ok.
if (PASSWORD_ARGON2_PROVIDER === 'standard') {
	$reject += include(__DIR__.'/argon2_malformed_hashes_unsafe.inc');
}

$time_s = hrtime(true);
password_verify('password', '$argon2id$v=19$m=8,t=1,p=1$SHRvL1pKdG9ubzBaaWdVdA$4xzDogL0DbOmWBXrIkitTD3SkuvvhSRFEctuEIQIp0o');
$time_e = hrtime(true);
$min_compute_time = ($time_e - $time_s);

echo sprintf("Verifying that %d invalid hashes are rejected ...", count($reject));
$ok = true;
foreach ($reject as $k => $test) {
	$time_s = hrtime(true);
	$return = password_verify('password', $test);
	$time_e = hrtime(true);
	$time_t = $time_e - $time_s;
	if ($return !== false) {
		echo sprintf("\ntrue returned for %s (%s) with password 'password'\n", $test, $k);
		break;
	} elseif ($time_t >= $min_compute_time) {
		$ok = false;
		echo sprintf(
			"\n%s (%s) took longer to reject than a minimal valid hash",
			$test, $k
		);
	}
}
echo ($ok) ? " OK\n" : "\n";
echo "OK!";
?>
--EXPECTF--
Verifying that %d invalid hashes are rejected ... OK
OK!
