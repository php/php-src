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
    if ($stream) {
        $meta = stream_get_meta_data($stream);
        var_dump($meta);
        var_dump(isset($meta['foo']) ? $meta['foo'] : null);
    }
}

?>
--EXPECTF--
array(7) {
  ["base64"]=>
  bool(false)
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
}
NULL

Warning: fopen(data://): Failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
array(7) {
  ["base64"]=>
  bool(true)
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
}
NULL

Warning: fopen(data://;base64): Failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d

Warning: fopen(data://foo,): Failed to open stream: rfc2397: illegal media type in %sstream_rfc2397_002.php on line %d

Warning: fopen(data://foo=bar,): Failed to open stream: rfc2397: illegal media type in %sstream_rfc2397_002.php on line %d
array(8) {
  ["mediatype"]=>
  string(10) "text/plain"
  ["base64"]=>
  bool(false)
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
}
NULL

Warning: fopen(data://text/plain;foo,): Failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
array(9) {
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["base64"]=>
  bool(false)
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
}
string(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bla,): Failed to open stream: rfc2397: illegal parameter in %sstream_rfc2397_002.php on line %d
array(9) {
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["base64"]=>
  bool(true)
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
}
string(3) "bar"

Warning: fopen(data://text/plain;foo=bar;bar=baz): Failed to open stream: rfc2397: no comma in URL in %sstream_rfc2397_002.php on line %d
array(10) {
  ["mediatype"]=>
  string(10) "text/plain"
  ["foo"]=>
  string(3) "bar"
  ["bar"]=>
  string(3) "baz"
  ["base64"]=>
  bool(false)
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
}
string(3) "bar"
