--TEST--
Test normal operation of password_get_info() with bcrypt-wrap
--FILE--
<?php
$template = '$bcrypt-wrap$v=%s$cost=%s,prehash=%s$%s$%s';
$goodvals = ["v" => "0", "cost" => "12", "prehash" => "cshake256", "salt" => "61bdXBR/Y1z+BJfnwDRpZQ", "hash" => "JY3BZsZi+7FTYa00u/s5CVSclj6hA6c"];

$missing = [
	"v"       => sprintf(str_replace("v=%s$", "", $template), $goodvals["cost"], $goodvals["prehash"], $goodvals["salt"], $goodvals["hash"]),
	"cost"    => sprintf(str_replace("cost=%s,", "", $template), $goodvals["v"], $goodvals["prehash"], $goodvals["salt"], $goodvals["hash"]),
	"prehash" => sprintf(str_replace(",prehash=%s", "", $template), $goodvals["v"], $goodvals["cost"], $goodvals["salt"], $goodvals["hash"]),
	"salt"    => sprintf(str_replace("$%s$", "$", $template), $goodvals["v"], $goodvals["cost"], $goodvals["prehash"], $goodvals["hash"]),
	"hash"    => sprintf(str_replace("$%s$%s", "$%s", $template), $goodvals["v"], $goodvals["cost"], $goodvals["prehash"], $goodvals["salt"]),
];

foreach ($missing as $name => $val) {
	$info = password_get_info($val);
	printf("missing %s :: %s\n", $name, $info["algoName"]);
}

foreach (["", " ", "0", "+", "-1", "10", PHP_INT_MAX] as $v) {
	$info = password_get_info(sprintf($template, $v, $goodvals["cost"], $goodvals["prehash"], $goodvals["salt"], $goodvals["hash"]));
	printf("v=%s :: %s\n", $v, $info["algoName"]);
}

foreach (["", " ", "-10", "3", "-4", "+4", " 4", "4", "31", "32"] as $cost) {
	$info = password_get_info(sprintf($template, $goodvals["v"], $cost, $goodvals["prehash"], $goodvals["salt"], $goodvals["hash"]));
	printf("cost=%s :: %s\n", $cost, $info["algoName"]);
}

foreach (["", " ", "cshake256", "CSHAKE256", "sha512", "SHA512", "sha-512", "sha256"] as $prehash) {
	$info = password_get_info(sprintf($template, $goodvals["v"], $goodvals["cost"], $prehash, $goodvals["salt"], $goodvals["hash"]));
	printf("prehash=%s :: %s\n", $prehash, $info["algoName"]);
}

$payloads = [
	"short salt" => ["salt" => substr($goodvals["salt"], 0, -1)],
	"long salt"  => ["salt" => $goodvals["salt"]."s"],
	"short hash" => ["hash" => substr($goodvals["hash"], 0, 1)],
	"long hash"  => ["hash" => $goodvals["hash"]."h"],
	"misaligned" => ["salt" => $goodvals["salt"]."s", "hash" => substr($goodvals["hash"], 0, -1)],
	"bad salt"   => ["salt" => substr($goodvals["salt"], 0, -1)."."],
	"bad hash"   => ["hash" => substr($goodvals["hash"], 0, -1)."."],
];

foreach ($payloads as $name => $val) {
	$info = password_get_info(sprintf($template, $goodvals["v"], $goodvals["cost"], $goodvals["prehash"], $val["salt"] ?? $goodvals["salt"], $val["hash"] ?? $goodvals["hash"]));
	printf("%s :: %s\n", $name, $info["algoName"]);
}

echo "OK!";
?>
--EXPECT--
missing v :: unknown
missing cost :: unknown
missing prehash :: unknown
missing salt :: unknown
missing hash :: unknown
v= :: unknown
v=  :: unknown
v=0 :: bcrypt-wrap
v=+ :: unknown
v=-1 :: unknown
v=10 :: unknown
v=9223372036854775807 :: unknown
cost= :: unknown
cost=  :: unknown
cost=-10 :: unknown
cost=3 :: unknown
cost=-4 :: unknown
cost=+4 :: unknown
cost= 4 :: unknown
cost=4 :: bcrypt-wrap
cost=31 :: bcrypt-wrap
cost=32 :: unknown
prehash= :: unknown
prehash=  :: unknown
prehash=cshake256 :: bcrypt-wrap
prehash=CSHAKE256 :: unknown
prehash=sha512 :: bcrypt-wrap
prehash=SHA512 :: unknown
prehash=sha-512 :: unknown
prehash=sha256 :: unknown
short salt :: unknown
long salt :: unknown
short hash :: unknown
long hash :: unknown
misaligned :: unknown
bad salt :: unknown
bad hash :: unknown
OK!
