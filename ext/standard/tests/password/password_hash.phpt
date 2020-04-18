--TEST--
Test normal operation of password_hash()
--FILE--
<?php
//-=-=-=-

var_dump(strlen(password_hash("foo", PASSWORD_BCRYPT)));

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
--EXPECT--
int(60)
bool(true)
bool(true)
bool(true)
OK!
