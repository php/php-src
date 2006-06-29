--TEST--
Stream: RFC2397 getting meta data
--FILE--
<?php

$streams = array(
	'data://,',
	'data://',
	'data://;base64,',
	'data://;base64',
	'data://foo,',
	'data://foo=bar,',
	'data://text/plain,',
	'data://text/plain;foo,',
	'data://text/plain;foo=bar,',
	'data://text/plain;foo=bar;bla,',
	'data://text/plain;foo=bar;base64,',
	'data://text/plain;foo=bar;bar=baz',
	'data://text/plain;foo=bar;bar=baz,',
	);

foreach($streams as $stream)
{
	$stream = fopen($stream, 'r');
	$meta = @stream_get_meta_data($stream);
	var_dump($meta);
	var_dump(isset($meta['foo']) ? $meta['foo'] : null);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(7) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(8) "data://,"
  ["base64"]=>
  bool(false)
}
NULL

Warning: fopen(data://): failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(7) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(15) "data://;base64,"
  ["base64"]=>
  bool(true)
}
NULL

Warning: fopen(data://;base64): failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
bool(false)
NULL

Warning: fopen(data://foo,): failed to open stream: rfc2397: illegal media type in %sstream_rfc2397_002.php on line %d
bool(false)
NULL

Warning: fopen(data://foo=bar,): failed to open stream: rfc2397: illegal media type in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(8) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(18) "data://text/plain,"
  ["mediatype"]=>
  string(10) "text/plain"
  ["base64"]=>
  bool(false)
}
NULL

Warning: fopen(data://text/plain;foo,): failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(9) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(26) "data://text/plain;foo=bar,"
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["base64"]=>
  bool(false)
}
string(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bla,): failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(9) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(33) "data://text/plain;foo=bar;base64,"
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["base64"]=>
  bool(true)
}
string(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bar=baz): failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(10) {
  ["wrapper_type"]=>
  string(7) "RFC2397"
  ["stream_type"]=>
  string(7) "RFC2397"
  ["mode"]=>
  string(1) "r"
  ["unread_bytes"]=>
  int(0)
  ["seekable"]=>
  bool(true)
  ["uri"]=>
  string(34) "data://text/plain;foo=bar;bar=baz,"
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "baz"
  ["base64"]=>
  bool(false)
}
string(3) "bar"
===DONE===
