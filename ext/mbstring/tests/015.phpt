--TEST--
mb_convert_variables()
--SKIPIF--
<?php include('skipif.inc'); ?>
--POST--
--GET--
--FILE--
<?php include('015.inc'); ?>
--EXPECT--
== SCALER TEST ==
SJIS
日本語テキストです。01234５６７８９。
JIS
日本語テキストです。01234５６７８９。
EUC-JP
k/qWe4zqg2WDTINYg2eCxYK3gUIwMTIzNIJUglWCVoJXgliBQg==
EUC-JP
GyRCRnxLXDhsJUYlLSU5JUgkRyQ5ISMbKEIwMTIzNBskQiM1IzYjNyM4IzkhIxsoQg==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== ARRAY TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== OBJECT TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
== SCALER, ARRAY AND OBJECT TEST ==
EUC-JP
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。
日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。日本語テキストです。01234５６７８９。

