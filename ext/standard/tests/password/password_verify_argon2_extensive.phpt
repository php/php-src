--TEST--
Test normal operation of password_verify() with Argon2i and Argon2id -- extensive
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2_PROVIDER')) die('skip password_verify not built with Argon2');
if (PASSWORD_ARGON2_PROVIDER === 'openssl') die('skip Argon2 provided by openssl');
if (getenv("SKIP_SLOW_TESTS")) die("skip slow test");
?>
--FILE--
<?php
$verify = [
	// RFC9106 has unusable test vectors as they include peppers and aad. The following are sourced from
	// https://github.com/P-H-C/phc-winner-argon2/blob/92cd2e19b50b9a58d177e61e1d8ce925315d3d91/src/test.c
	// Vectors using over 65MiB excluded to not overload CI:
	//	$argon2i$m=1048576,t=2,p=1$c29tZXNhbHQ$lpDsVdKNPtMlYvLnPqYrArAYdXZDoq5ueVKEWd6BBuk (v1.0 1GiB)
	//	$argon2i$m=262144,t=2,p=1$c29tZXNhbHQ$Pmiaqj0op3zyvHKlGsUxZnYXURgvHuKS4/Z3p9pMJGc (v1.0 256MiB)
	//	$argon2i$v=19$m=1048576,t=2,p=1$c29tZXNhbHQ$0Vh6ygkiw7XWqD7asxvuPE667zQu1hJ6VdGbI1GtH0E (v1.3 1GiB argon2i)
	//	$argon2i$v=19$m=262144,t=2,p=1$c29tZXNhbHQ$KW266AuAfNzqrUSudBtQbxTbCVkmexg7EY+bJCKbx8s (v1.3 256MiB argon2i)
	//	$argon2id$v=19$m=262144,t=2,p=1$c29tZXNhbHQ$eP4eyR+zqlZX1y5xCFTkw9m5GYx0L5YWwvCFvtlbLow (v1.3 256MiB argon2id)

	// Version 1.0
// These are currently rejected by both openssl and libsodium; WIP
//	['hash' => '$argon2i$m=65536,t=2,p=1$c29tZXNhbHQ$9sTbSlTio3Biev89thdrlKKiCaYsjjYVJxGAL3swxpQ'],
//	['hash' => '$argon2i$m=256,t=2,p=1$c29tZXNhbHQ$/U3YPXYsSb3q9XxHvc0MLxur+GP960kN9j7emXX8zwY'],
//	['hash' => '$argon2i$m=256,t=2,p=2$c29tZXNhbHQ$tsEVYKap1h6scGt5ovl9aLRGOqOth+AMB+KwHpDFZPs'],
//	['hash' => '$argon2i$m=65536,t=1,p=1$c29tZXNhbHQ$gWMFUrjzsfSM2xmSxMZ4ZD1JCytetP9sSzQ4tWIXJLI'],
//	['hash' => '$argon2i$m=65536,t=4,p=1$c29tZXNhbHQ$8hLwFhXm6110c03D70Ct4tUdBSRo2MaUQKOh8sHChHs'],
//	['hash' => '$argon2i$m=65536,t=2,p=1$c29tZXNhbHQ$6ckCB0tnVFMaOgvlGeW69ASzDOabPwGsO/ISKZYBCaM', 'password' => 'differentpassword'],
//	['hash' => '$argon2i$m=65536,t=2,p=1$ZGlmZnNhbHQ$eaEDuQ/orvhXDLMfyLIiWXeJFvgza3vaw4kladTxxJc'],
//	['hash' => '$argon2i$m=65536,t=2,p=1$c29tZXNhbHQ$b2G3seW+uPzerwQQC+/E1K50CLLO7YXy0JRcaTuswRo', 'expect' => false], // password is 'passwore'
	// Version 1.3
	['hash' => '$argon2i$v=19$m=65536,t=2,p=1$c29tZXNhbHQ$wWKIMhR9lyDFvRz9YTZweHKfbftvj+qf+YFY4NeBbtA'],
	['hash' => '$argon2i$v=19$m=256,t=2,p=1$c29tZXNhbHQ$iekCn0Y3spW+sCcFanM2xBT63UP2sghkUoHLIUpWRS8'],
	['hash' => '$argon2i$v=19$m=256,t=2,p=2$c29tZXNhbHQ$T/XOJ2mh1/TIpJHfCdQan76Q5esCFVoT5MAeIM1Oq2E'],
	['hash' => '$argon2i$v=19$m=65536,t=1,p=1$c29tZXNhbHQ$0WgHXE2YXhPr6uVgz4uUw7XYoWxRkWtvSsLaOsEbvs8'],
	['hash' => '$argon2i$v=19$m=65536,t=4,p=1$c29tZXNhbHQ$qqlT1YrzcGzj3xrv1KZKhOMdf1QXUjHxKFJZ+IF0zls'],
	['hash' => '$argon2i$v=19$m=65536,t=2,p=1$c29tZXNhbHQ$FK6NoBr+qHAMI1jc73xTWNkCEoK9iGY6RWL1n7dNIu4', 'password' => 'differentpassword'],
	['hash' => '$argon2i$v=19$m=65536,t=2,p=1$ZGlmZnNhbHQ$sDV8zPvvkfOGCw26RHsjSMvv7K2vmQq/6cxAcmxSEnE'],
	['hash' => '$argon2i$v=19$m=65536,t=2,p=1$c29tZXNhbHQ$8iIuixkI73Js3G1uMbezQXD0b8LG4SXGsOwoQkdAQIM', 'expect' => false], // password is 'passwore'
	['hash' => '$argon2id$v=19$m=65536,t=2,p=1$c29tZXNhbHQ$CTFhFdXPJO1aFaMaO6Mm5c8y7cJHAph8ArZWb2GRPPc'],
	['hash' => '$argon2id$v=19$m=256,t=2,p=1$c29tZXNhbHQ$nf65EOgLrQMR/uIPnA4rEsF5h7TKyQwu9U1bMCHGi/4'],
	['hash' => '$argon2id$v=19$m=256,t=2,p=2$c29tZXNhbHQ$bQk8UB/VmZZF4Oo79iDXuL5/0ttZwg2f/5U52iv1cDc'],
	['hash' => '$argon2id$v=19$m=65536,t=1,p=1$c29tZXNhbHQ$9qWtwbpyPd3vm1rB1GThgPzZ3/ydHL92zKL+15XZypg'],
	['hash' => '$argon2id$v=19$m=65536,t=4,p=1$c29tZXNhbHQ$kCXUjmjvc5XMqQedpMTsOv+zyJEf5PhtGiUghW9jFyw'],
	['hash' => '$argon2id$v=19$m=65536,t=2,p=1$c29tZXNhbHQ$C4TWUs9rDEvq7w3+J4umqA32aWKB1+DSiRuBfYxFj94', 'password' => 'differentpassword'],
	['hash' => '$argon2id$v=19$m=65536,t=2,p=1$ZGlmZnNhbHQ$vfMrBczELrFdWP0ZsfhWsRPaHppYdP3MVEMIVlqoFBw'],
	// --- END upstream test vectors ---
];

echo sprintf("Verifying %d valid hashes ...", count($verify));
$ok = true;
foreach ($verify as $k => $test) {
	$password = $test['password'] ?? 'password';
	// Please don't try to combine these into a single expression; it hides unexpected values upon failure
	$expect   = $test['expect'] ?? true;
	$return   = password_verify($password, $test['hash']);
	if ($return !== $expect) {
		$ok = false;
		echo sprintf(
			"\n%s returned for (index %d) %s with password '%s'",
			var_export($return, true), $k, $test['hash'], $password
		);
	}
}
echo ($ok) ? " OK\n" : "\n";
echo "OK!";
?>
--EXPECTF--
Verifying %d valid hashes ... OK
OK!
