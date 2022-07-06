--TEST--
ZipArchive::setEncryption*() functions
--SKIPIF--
<?php
if (!extension_loaded('zip')) die('skip');
if (!method_exists('ZipArchive', 'setEncryptionName')) die('skip encryption not supported');
?>
--FILE--
<?php

$name = __DIR__ . '/encrypted.zip';
$pass = 'secret';

echo "== Write\n";
$zip  = new ZipArchive;
$r = $zip->open($name, ZIPARCHIVE::CREATE);
// Clear
$zip->addFromString('foo.txt', 'foo');
// Encrypted
$zip->addFromString('bar.txt', 'bar');
var_dump($zip->setEncryptionName('bar.txt', 9999, $pass)); // Fails
var_dump($zip->setEncryptionName('bar.txt', ZipArchive::EM_AES_256, $pass));
$zip->close();

echo "== Read\n";
$r = $zip->open($name);
$s = $zip->statName('foo.txt');
var_dump($s['encryption_method'] === ZipArchive::EM_NONE);
$s = $zip->statName('bar.txt');
var_dump($s['encryption_method'] === ZipArchive::EM_AES_256);
var_dump($zip->getFromName('foo.txt')); // Clear, ok
var_dump($zip->getFromName('bar.txt')); // Encrypted, fails
$zip->setPassword($pass);
var_dump($zip->getFromName('bar.txt')); // Ecnrypted, ok
$zip->close();

echo "== Stream\n";
var_dump(file_get_contents("zip://$name#foo.txt")); // Clear, ok
var_dump(file_get_contents("zip://$name#bar.txt")); // Encrypted, fails
$ctx = stream_context_create(array('zip' => array('password' => $pass)));
var_dump(file_get_contents("zip://$name#bar.txt", false, $ctx)); // Ecnrypted, ok
?>
== Done
--CLEAN--
<?php
$name = __DIR__ . '/encrypted.zip';
@unlink($name);
?>
--EXPECTF--
== Write
bool(false)
bool(true)
== Read
bool(true)
bool(true)
string(3) "foo"
bool(false)
string(3) "bar"
== Stream
string(3) "foo"

Warning: file_get_contents(%s): Failed to open stream: operation failed in %s on line %d
bool(false)
string(3) "bar"
== Done
