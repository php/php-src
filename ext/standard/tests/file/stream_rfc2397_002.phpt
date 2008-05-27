--TEST--
Stream: RFC2397 getting meta data
--INI--
allow_url_fopen=1
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
array(8) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(8) "data://,"
  [u"base64"]=>
  bool(false)
}
NULL

Warning: fopen(data://): failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(8) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(15) "data://;base64,"
  [u"base64"]=>
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
array(9) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(18) "data://text/plain,"
  [u"mediatype"]=>
  unicode(10) "text/plain"
  [u"base64"]=>
  bool(false)
}
NULL

Warning: fopen(data://text/plain;foo,): failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(10) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(26) "data://text/plain;foo=bar,"
  [u"mediatype"]=>
  unicode(10) "text/plain"
  [u"foo"]=>
  unicode(3) "bar"
  [u"base64"]=>
  bool(false)
}
unicode(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bla,): failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(10) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(33) "data://text/plain;foo=bar;base64,"
  [u"mediatype"]=>
  unicode(10) "text/plain"
  [u"foo"]=>
  unicode(3) "bar"
  [u"base64"]=>
  bool(true)
}
unicode(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bar=baz): failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
bool(false)
NULL
array(11) {
  [u"wrapper_type"]=>
  unicode(7) "RFC2397"
  [u"stream_type"]=>
  unicode(7) "RFC2397"
  [u"mode"]=>
  unicode(1) "r"
  [u"unread_bytes"]=>
  int(0)
  [u"unread_chars"]=>
  int(0)
  [u"seekable"]=>
  bool(true)
  [u"uri"]=>
  unicode(34) "data://text/plain;foo=bar;bar=baz,"
  [u"mediatype"]=>
  unicode(10) "text/plain"
  [u"foo"]=>
  unicode(3) "bar"
  [u"bar"]=>
  unicode(3) "baz"
  [u"base64"]=>
  bool(false)
}
unicode(3) "bar"
===DONE===
