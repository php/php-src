--TEST--
Test normal operation of password_needs_rehash() with bcrypt-wrap
--FILE--
<?php
$current_version = 0;
$default_prehash = "cshake256";
$template = '$bcrypt-wrap$v=%s$cost=%s,prehash=%s$61bdXBR/Y1z+BJfnwDRpZQ$JY3BZsZi+7FTYa00u/s5CVSclj6hA6c';

for ($v = 0; $v <= $current_version; $v++) {
	$hash   = sprintf($template, $v, 4, "cshake256");
	$result = var_export(password_needs_rehash($hash, PASSWORD_BCRYPT_WRAP, ["cost" => 4, "prehash" => "cshake256"]), true);
	echo "version {$v} :: {$result}\n";
}

// Descriptions refer to the hash being parsed
$tests = [
	"same cost"    => ["cost" => [4, 4]],
	"lower cost"   => ["cost" => [4, 31]],
	"higher cost"  => ["cost" => [5, 4]],
	"cshake256 to cshake256" => ["prehash" => ["cshake256", "cshake256"]],
	"cshake256 to sha512"    => ["prehash" => ["cshake256", "sha512"]],
	"sha512 to sha512"       => ["prehash" => ["sha512", "sha512"]],
	"sha512 to cshake256"    => ["prehash" => ["sha512", "cshake256"]],
];

foreach ($tests as $name => $options) {
	$hash   = sprintf($template, $current_version, $options["cost"][0] ?? PASSWORD_BCRYPT_DEFAULT_COST, $options["prehash"][0] ?? $default_prehash);
	$result = var_export(password_needs_rehash($hash, PASSWORD_BCRYPT_WRAP, [
		"cost"    => $options["cost"][1] ?? PASSWORD_BCRYPT_DEFAULT_COST,
		"prehash" => $options["prehash"][1] ?? $default_prehash
	]), true);
	echo "{$name} :: {$result}\n";
}

echo "OK!";
?>
--EXPECT--
version 0 :: false
same cost :: false
lower cost :: true
higher cost :: true
cshake256 to cshake256 :: false
cshake256 to sha512 :: true
sha512 to sha512 :: false
sha512 to cshake256 :: true
OK!
