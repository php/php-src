--TEST--
Bug #52941 (The 'iconv_mime_decode_headers' function is skipping headers)
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
$headers = <<<HEADERS
From: =?UTF-8?B?PGZvb0BleGFtcGxlLmNvbT4=?=
Subject: =?ks_c_5601-1987?B?UkU6odk=?=
X-Foo: =?ks_c_5601-1987?B?UkU6odk=?= Foo
X-Bar: =?ks_c_5601-1987?B?UkU6odk=?= =?UTF-8?Q?Foo?=
To: <test@example.com>
HEADERS;

$decoded = iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_CONTINUE_ON_ERROR, 'UTF-8');

var_dump($decoded['From']);
var_dump($decoded['Subject']);
var_dump($decoded['X-Foo']);
var_dump($decoded['X-Bar']);
var_dump($decoded['To']);

$decoded = iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_CONTINUE_ON_ERROR | ICONV_MIME_DECODE_STRICT, 'UTF-8');

var_dump($decoded['From']);
var_dump($decoded['Subject']);
var_dump($decoded['X-Foo']);
var_dump($decoded['X-Bar']);
var_dump($decoded['To']);
?>
--EXPECTF--
string(17) "<foo@example.com>"
string(%d) "%s"
string(%d) "%sFoo"
string(%d) "%sFoo"
string(18) "<test@example.com>"
string(17) "<foo@example.com>"
string(%d) "%s"
string(%d) "%sFoo"
string(%d) "%sFoo"
string(18) "<test@example.com>"
