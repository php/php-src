--TEST--
crypt() bcrypt distinguishes passwords containing null bytes
--SKIPIF--
<?php
$setting = '$2y$05$CCCCCCCCCCCCCCCCCCCCC.';
if (crypt("foo\0bar", $setting) === crypt("foo", $setting)) {
    die("skip bcrypt backend truncates NUL bytes");
}
?>
--FILE--
<?php
$setting = '$2y$05$CCCCCCCCCCCCCCCCCCCCC.';
$hash = crypt("foo\0bar", $setting);

var_dump($hash === crypt("foo\0bar", $hash));
var_dump($hash === crypt("foo", $hash));
var_dump($hash === crypt("foo\0baz", $hash));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
