--TEST--
PDO_DBLIB: Ensure quote function returns expected results
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require __DIR__ . '/config.inc';
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

var_dump($db->quote(42));
var_dump($db->quote('42', PDO::PARAM_INT));
var_dump($db->quote('4.2', PDO::PARAM_INT));
var_dump($db->quote(true));
var_dump($db->quote(false));
var_dump($db->quote(true, PDO::PARAM_BOOL));
var_dump($db->quote(false, PDO::PARAM_BOOL));
var_dump($db->quote('42', PDO::PARAM_BOOL));
var_dump($db->quote('', PDO::PARAM_BOOL));
var_dump($db->quote(' ', PDO::PARAM_BOOL));
var_dump($db->quote('0', PDO::PARAM_BOOL));
var_dump($db->quote(null));
var_dump($db->quote(null, PDO::PARAM_NULL));
var_dump($db->quote('Съешь же ещё этих мягких французских булок, да выпей чаю.'));
var_dump($db->quote('The quick brown fox jumps over the lazy dog.'));
var_dump($db->quote(hex2bin('90243445b265537314e25b1a0ef96e9d'), PDO::PARAM_LOB));
var_dump($db->quote("foo'bar"));

?>
--EXPECT--
string(4) "'42'"
string(2) "42"
string(5) "'4.2'"
string(3) "'1'"
string(2) "''"
string(3) "'1'"
string(3) "'0'"
string(3) "'1'"
string(3) "'0'"
string(3) "'0'"
string(3) "'0'"
string(2) "''"
string(2) "''"
string(106) "N'Съешь же ещё этих мягких французских булок, да выпей чаю.'"
string(46) "'The quick brown fox jumps over the lazy dog.'"
string(34) "0x90243445b265537314e25b1a0ef96e9d"
string(10) "'foo''bar'"
