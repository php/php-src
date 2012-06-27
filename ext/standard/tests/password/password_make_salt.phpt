--TEST--
Test normal operation of password_make_salt()
--FILE--
<?php
//-=-=-=-
echo strlen(password_make_salt(1)) . "\n";
echo strlen(password_make_salt(2)) . "\n";
echo strlen(password_make_salt(3)) . "\n";
echo strlen(password_make_salt(4)) . "\n";
echo strlen(password_make_salt(5)) . "\n";
echo "\n";

echo strlen(password_make_salt(1, true)) . "\n";
echo strlen(password_make_salt(2, true)) . "\n";
echo strlen(password_make_salt(3, true)) . "\n";
echo strlen(password_make_salt(4, true)) . "\n";
echo strlen(password_make_salt(5, true)) . "\n";
echo "\n";

$a = password_make_salt(32);
$b = password_make_salt(32);

var_dump($a != $b);
echo "OK!";
?>
--EXPECT--
1
2
3
4
5

1
2
3
4
5

bool(true)
OK!
