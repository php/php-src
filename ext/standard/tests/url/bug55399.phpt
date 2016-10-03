--TEST--
Bug #55399 (parse_url() incorrectly treats ':' as a valid path)
--FILE--
<?php

var_dump(parse_url(":"));

?>
--EXPECT--
array(1) {
  ["path"]=>
  string(1) ":"
}
