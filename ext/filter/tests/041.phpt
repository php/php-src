--TEST--
COOKIE multiple cookie test
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--INI--
filter.default=stripped
filter.default_flags=0
--COOKIE--
abc=dir; def=true; abc=root; xyz="foo bar";
--FILE--
<?php
var_dump($_COOKIE);
var_dump(filter_has_var(INPUT_COOKIE, "abc"));
var_dump(filter_input(INPUT_COOKIE, "abc"));
var_dump(filter_input(INPUT_COOKIE, "def"));
var_dump(filter_input(INPUT_COOKIE, "xyz"));
var_dump(filter_has_var(INPUT_COOKIE, "bogus"));
var_dump(filter_input(INPUT_COOKIE, "xyz", FILTER_SANITIZE_SPECIAL_CHARS));
?>
--EXPECT--
array(3) {
  ["abc"]=>
  string(3) "dir"
  ["def"]=>
  string(4) "true"
  ["xyz"]=>
  string(17) "&#34;foo bar&#34;"
}
bool(true)
string(3) "dir"
string(4) "true"
string(9) ""foo bar""
bool(false)
string(17) "&#34;foo bar&#34;"
