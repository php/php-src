--TEST--
Test normal operation of password_hash() with bcrypt_wrap
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php


var_dump(password_hash("foo", PASSWORD_BCRYPT_WRAP));
var_dump(password_hash("foo", "bcrypt-wrap", ["cost" => 4, "prehash" => "cshake256"]));
var_dump($hash = password_hash("foo", 4, ["cost" => 4, "prehash" => "sha512"]));
var_dump(password_get_info($hash)['algo']);

$nullpass = "null\x00byte";
$longpass = str_repeat("a", 72);

foreach (["cshake256", "sha512"] as $prehash) {
	$hash = password_hash($nullpass, PASSWORD_BCRYPT_WRAP, ["cost" => 4, "prehash" => $prehash]);
	var_dump(password_verify($nullpass, $hash));
	var_dump(password_verify($nullpass."suffix", $hash));
	$hash = password_hash($longpass."suffix", PASSWORD_BCRYPT_WRAP, ["cost" => 4, "prehash" => $prehash]);
	var_dump(password_verify($longpass."suffix", $hash));
	var_dump(password_verify($longpass."suffix2", $hash));
	var_dump(password_get_info($hash)['options']['prehash']);
}

echo "OK!";
?>
--EXPECTF--
string(97) "$bcrypt-wrap$v=0$cost=12,prehash=cshake256$%s$%s"
string(96) "$bcrypt-wrap$v=0$cost=4,prehash=cshake256$%s$%s"
string(93) "$bcrypt-wrap$v=0$cost=4,prehash=sha512$%s$%s"
string(11) "bcrypt-wrap"
bool(true)
bool(false)
bool(true)
bool(false)
string(9) "cshake256"
bool(true)
bool(false)
bool(true)
bool(false)
string(6) "sha512"
OK!
