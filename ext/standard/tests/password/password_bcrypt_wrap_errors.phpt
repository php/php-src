--TEST--
Test error operation of password_hash() with bcrypt-wrap
--FILE--
<?php
$params = [
	["cost" => 3],
	["cost" => 32],
	["cost" => 4, "prehash" => "sha256"]
];

foreach ($params as $p) {
	try {
		password_hash("foo", PASSWORD_BCRYPT_WRAP, $p);
	} catch (Throwable $exception) {
		echo $exception::class, ': ', $exception->getMessage(), "\n";
	}
}

try {
	password_hash(str_repeat("f", 1025), PASSWORD_BCRYPT_WRAP, ["cost"=>4]);
} catch (Throwable $exception) {
	echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: Invalid bcrypt cost parameter specified: 3
ValueError: Invalid bcrypt cost parameter specified: 32
ValueError: Invalid bcrypt-wrap prehash parameter specified: sha256
ValueError: Bcrypt-wrap password must not be larger than 1KiB
