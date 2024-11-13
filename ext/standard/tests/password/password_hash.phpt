--TEST--
Test normal operation of password_hash()
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow test"); ?>
--FILE--
<?php
//-=-=-=-

var_dump(password_hash("foo", PASSWORD_BCRYPT));

$algos = [
  PASSWORD_BCRYPT,
  '2y',
  1,
];

foreach ($algos as $algo) {
  $hash = password_hash("foo", $algo);
  var_dump($hash === crypt("foo", $hash));
}

echo "OK!";
?>
--EXPECTF--
string(60) "$2y$12$%s"
bool(true)
bool(true)
bool(true)
OK!
