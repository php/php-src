--TEST--
Bug #49692: parse_ini_file() throws errors when key contains '/' (forward slash)
--CREDITS--
Rafael Dohms <rdohms [at] gmail [dot] com>
--FILE--
<?php

var_dump(parse_ini_file('bug49692.ini', true));

?>
--EXPECTF--
array(1) {
  [u"sitemap"]=>
  array(2) {
    [u"/home"]=>
    unicode(13) "default:index"
    [u"/info"]=>
    unicode(12) "default:info"
  }
}
