--TEST--
GH-12703 (parse_url mishandles colon inside path)
--FILE--
<?php
// Case 1 (issue report): an absolute path with a colon and no query string
// used to return false instead of the path.
var_dump(parse_url('/page:1'));
var_dump(parse_url('/page:1', PHP_URL_PATH));
var_dump(parse_url('/page:1', PHP_URL_SCHEME));

// A query string already worked via a different branch, keep it as a regression guard.
var_dump(parse_url('/page:1?foo=bar'));

// Pathological single-slash inputs that exercise the new branch path.
var_dump(parse_url('/:'));
var_dump(parse_url('/:80'));

// Case 2: a relative-scheme URL (//host/path) with colon-like digits inside
// the path used to strip the digits out as a phantom port. The host has no
// explicit port in these inputs, so parse_url should not report one.
var_dump(parse_url('//www.example.com/foo:65535/'));
var_dump(parse_url('//www.example.com/foo:1/'));
var_dump(parse_url('//www.example.com/foo:65536/'));
var_dump(parse_url('//host/a:1/b:2/'));

// Explicit host port must still be extracted, and the colon inside the path
// must stay inside the path.
var_dump(parse_url('//www.example.com:8080/foo:65535/'));

// Full URL with scheme, auth, host, port, path with colon, query, fragment
// must still round-trip correctly.
var_dump(parse_url('scheme://user:pass@host:8080/a:1/b:2?q=1#f'));
?>
--EXPECT--
array(1) {
  ["path"]=>
  string(7) "/page:1"
}
string(7) "/page:1"
NULL
array(2) {
  ["path"]=>
  string(7) "/page:1"
  ["query"]=>
  string(7) "foo=bar"
}
array(1) {
  ["path"]=>
  string(2) "/:"
}
array(1) {
  ["path"]=>
  string(4) "/:80"
}
array(2) {
  ["host"]=>
  string(15) "www.example.com"
  ["path"]=>
  string(11) "/foo:65535/"
}
array(2) {
  ["host"]=>
  string(15) "www.example.com"
  ["path"]=>
  string(7) "/foo:1/"
}
array(2) {
  ["host"]=>
  string(15) "www.example.com"
  ["path"]=>
  string(11) "/foo:65536/"
}
array(2) {
  ["host"]=>
  string(4) "host"
  ["path"]=>
  string(9) "/a:1/b:2/"
}
array(3) {
  ["host"]=>
  string(15) "www.example.com"
  ["port"]=>
  int(8080)
  ["path"]=>
  string(11) "/foo:65535/"
}
array(8) {
  ["scheme"]=>
  string(6) "scheme"
  ["host"]=>
  string(4) "host"
  ["port"]=>
  int(8080)
  ["user"]=>
  string(4) "user"
  ["pass"]=>
  string(4) "pass"
  ["path"]=>
  string(8) "/a:1/b:2"
  ["query"]=>
  string(3) "q=1"
  ["fragment"]=>
  string(1) "f"
}
