--TEST--
mb_convert_encoding()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('016.inc'); ?>
--EXPECT--
== BASIC TEST ==
EUC-JP: 日本語テキストです。01234５６７８９。
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== ARRAY ENCODING LIST ==
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== DETECT ORDER ==
EUC-JP: 日本語テキストです。01234５６７８９。
SJIS: k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
JIS: GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
== INVALID PARAMETER ==
INT: 1234
EUC-JP: 
ERR: Warning
BAD: 
ERR: Warning
MP: 


