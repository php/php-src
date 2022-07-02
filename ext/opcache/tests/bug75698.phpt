--TEST--
Bug #75698: Using @ crashes php7.2-fpm
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php

function test() {
  $a = array("a","b","c","b");
  $b = array();
  foreach ($a as $c)
    @$b[$c]++; // the @ is required to crash PHP 7.2.0
  var_dump($b);
}

test();

?>
--EXPECT--
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(1)
}
