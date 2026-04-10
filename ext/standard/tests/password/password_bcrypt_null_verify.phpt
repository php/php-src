--TEST--
password_* handles bcrypt passwords containing null bytes
--SKIPIF--
<?php
$setting = '$2y$05$CCCCCCCCCCCCCCCCCCCCC.';
if (crypt("foo\0bar", $setting) === crypt("foo", $setting)) {
    die("skip bcrypt backend truncates NUL bytes");
}
?>
--FILE--
<?php
$password = "foo\0bar";
$hash = password_hash($password, PASSWORD_BCRYPT);

var_dump($hash !== false);
var_dump(password_verify($password, $hash));
var_dump(password_verify("foo", $hash));
var_dump(password_verify("foo\0baz", $hash));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
