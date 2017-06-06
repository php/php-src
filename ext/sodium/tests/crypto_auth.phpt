--TEST--
Check for libsodium auth
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$msg = random_bytes(1000);
$key = random_bytes(SODIUM_CRYPTO_AUTH_KEYBYTES);
$mac = sodium_crypto_auth($msg, $key);

// This should validate
var_dump(sodium_crypto_auth_verify($mac, $msg, $key));

$bad_key = random_bytes(SODIUM_CRYPTO_AUTH_KEYBYTES - 1);
try {
	$mac = sodium_crypto_auth($msg, $bad_key);
	echo 'Fail!', PHP_EOL;
} catch (SodiumException $ex) {
  echo $ex->getMessage(), PHP_EOL;
}

// Flip the first bit
$badmsg = $msg;
$badmsg[0] = \chr(\ord($badmsg[0]) ^ 0x80);
var_dump(sodium_crypto_auth_verify($mac, $badmsg, $key));

// Let's flip a bit pseudo-randomly
$badmsg = $msg;
$badmsg[$i=mt_rand(0, 999)] = \chr(
	\ord($msg[$i]) ^ (
		1 << mt_rand(0, 7)
	)
);

var_dump(sodium_crypto_auth_verify($mac, $badmsg, $key));

// Now let's change a bit in the MAC
$badmac = $mac;
$badmac[0] = \chr(\ord($badmac[0]) ^ 0x80);
var_dump(sodium_crypto_auth_verify($badmac, $msg, $key));
?>
--EXPECT--
bool(true)
crypto_auth(): key must be CRYPTO_AUTH_KEYBYTES bytes
bool(false)
bool(false)
bool(false)
