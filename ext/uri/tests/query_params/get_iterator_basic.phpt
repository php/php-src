--TEST--
Test Uri\QueryParams::getIterator() - success - basic
--FILE--
<?php

$params = Uri\QueryParams::parseRfc3986("foo=123&foo=456&bar=789");

foreach ($params as $entry) {
    var_dump($entry);
}

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "123"
}
array(2) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "456"
}
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  string(3) "789"
}
