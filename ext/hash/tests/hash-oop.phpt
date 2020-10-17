--TEST--
Hash: OOP API
--FILE--
<?php

// Basic usage.
$ctx = new HashContext('sha1');
var_dump($ctx->getAlgo());
$ctx->update("I can't remember anything");
var_dump($ctx->final());

// Fluent usage.
var_dump('sha256');
var_dump((new HashContext('sha256'))->update("I can't remember anything")->final());

// Cloned context.
$ctx = (new HashContext('sha512'))->update("I can't remember anything");
var_dump($ctx->getAlgo());
var_dump((clone $ctx)->final());
var_dump($ctx->final());

// HMAC
var_dump('ripemd128');
var_dump((new HashContext('ripemd128', HASH_HMAC, 'secret'))->update("I can't remember anything")->final());

// Update from stream.
var_dump('stream-md5');
$fp = tmpfile();
fwrite($fp, 'The quick brown fox jumped over the lazy dog.');
rewind($fp);
var_dump((new HashContext('md5'))->updateStream($fp)->final());

// Update from file.
var_dump('file-md5');
$filePath = __DIR__ . DIRECTORY_SEPARATOR . 'hash-oop-file.txt';
file_put_contents($filePath, 'The quick brown fox jumped over the lazy dog.');
var_dump((new HashContext('md5'))->updateFile($filePath)->final());

// Update from non-existant file.
var_dump('file-not-found');
try {
	var_dump((new HashContext('md5'))->updateFile(__DIR__ . DIRECTORY_SEPARATOR . 'does-not-exist.txt')->final());
} catch (Throwable $ex) {
	var_dump($ex->getMessage());
}

--EXPECTF--
string(4) "sha1"
string(40) "29f62a228f726cd728efa7a0ac6a2aba318baf15"
string(6) "sha256"
string(64) "d3a13cf52af8e9390caed78b77b6b1e06e102204e3555d111dfd149bc5d54dba"
string(6) "sha512"
string(128) "caced3db8e9e3a5543d5b933bcbe9e7834e6667545c3f5d4087b58ec8d78b4c8a4a5500c9b88f65f7368810ba9905e51f1cff3b25a5dccf76634108fb4e7ce13"
string(128) "caced3db8e9e3a5543d5b933bcbe9e7834e6667545c3f5d4087b58ec8d78b4c8a4a5500c9b88f65f7368810ba9905e51f1cff3b25a5dccf76634108fb4e7ce13"
string(9) "ripemd128"
string(32) "7446def1c89d9b8a9bffdb6e134ccb9c"
string(10) "stream-md5"
string(32) "5c6ffbdd40d9556b73a21e63c3e0e904"
string(8) "file-md5"
string(32) "5c6ffbdd40d9556b73a21e63c3e0e904"
string(14) "file-not-found"
string(%s) "Error opening '%sdoes-not-exist.txt'"
--CLEAN--
<?php
unlink(__DIR__ . DIRECTORY_SEPARATOR . 'hash-oop-file.txt');
