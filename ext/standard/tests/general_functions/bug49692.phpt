--TEST--
Bug #49692: parse_ini_file() throws errors when key contains '/' (forward slash)
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--FILE--
<?php

var_dump(parse_ini_file('bug49692.ini', true));

?>
--EXPECT--
array(1) {
  ["sitemap"]=>
  array(2) {
    ["/home"]=>
    string(13) "default:index"
    ["/info"]=>
    string(12) "default:info"
  }
}
