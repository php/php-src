--TEST--
GH-12703 parse_url() return false on absolute path containing ':' and no query string.
--FILE--
<?php
var_dump(
    parse_url('/page:1'),
    parse_url('/page:1', \PHP_URL_SCHEME),

    // Working example for context.
    parse_url('/page:1?foo=bar'),

    // Examples from similar GH issue (7890)
    parse_url('//www.examle.com/foo:65535/'),
    parse_url('//www.examle.com:8080/foo:65535/'),
);

?>
--EXPECT--
array(1) {
  ["path"]=>
  string(7) "/page:1"
}
NULL
array(2) {
  ["path"]=>
  string(7) "/page:1"
  ["query"]=>
  string(7) "foo=bar"
}
array(2) {
  ["host"]=>
  string(14) "www.examle.com"
  ["path"]=>
  string(11) "/foo:65535/"
}
array(3) {
  ["host"]=>
  string(14) "www.examle.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(11) "/foo:65535/"
}
