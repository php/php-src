--TEST--
Test normal operation of password_verify() with bcrypt-wrap
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php
// One double-digit cost test to cover the basics, but keep the rest fast when possible
$tests = [
	[
		"pass" => "foo",
		"hash" => '$bcrypt-wrap$v=0$cost=10,prehash=cshake256$bfJceq0aXoBAI28hkEyTDA$BhYZPQcj3KnqdpKaK/Xd3Vg09a1HC0A'
	],
	[
		"pass" => "\00\x01\x02\x03",
		"hash" => '$bcrypt-wrap$v=0$cost=4,prehash=cshake256$eT1y0ul8SrOZlfBE7JlpNQ$MK6JKABH7ToMyIvuXS0a4UhPzW5Av+c'
	],
	[
		"pass" => str_repeat("f", 73),
		"hash" => '$bcrypt-wrap$v=0$cost=5,prehash=sha512$ditAGtUdwL761dx7dFyIsQ$ELKJD3syWZZ6tA/OcB8Sv+UvYTw/shk'
	],
];

foreach ($tests as $t) {
	var_dump(password_verify($t["pass"], $t["hash"]));
	var_dump(password_verify($t["pass"]."\x00\x46", $t["hash"]));
}

$limit_pass = str_repeat("f", 1024);
$limit_hash = '$bcrypt-wrap$v=0$cost=4,prehash=cshake256$G26CVD/4p2TIBlGkLmR3XQ$2MrQUpU2LI0k0HmIDKhVtmZUHyigmUU';

$computed_s = hrtime(true);
var_dump(password_verify($limit_pass, $limit_hash));
$computed_e = hrtime(true);
$computed_t = $computed_e - $computed_s;

$rejected_s = hrtime(true);
var_dump(password_verify($limit_pass."f", $limit_hash));
$rejected_e = hrtime(true);
$rejected_t = $rejected_e - $rejected_s;

// The actual ratio is many times over, since no heavy computation would've occured
var_dump(($computed_t / $rejected_t) > 10);

echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
OK!
