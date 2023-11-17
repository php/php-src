--TEST--
Bug #X	parse_url() return false on absolute path containing ':' and no query string.
--FILE--
<?php
var_dump(
    parse_url('/page:1'),
    parse_url('/page:1', \PHP_URL_SCHEME)
);

?>
--EXPECT--
array(1) {
  ["path"]=>
  string(7) "/page:1"
}
NULL
