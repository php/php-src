--TEST--
filter.default with parse_str must not crash on non-string filter results
--EXTENSIONS--
filter
--INI--
filter.default=int
--FILE--
<?php
parse_str('a=1&b=notint', $out);
var_dump($out);
?>
--EXPECTF--
Deprecated: The filter.default ini setting is deprecated in %s on line %d
array(2) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(0) ""
}
