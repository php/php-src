--TEST--
GH-11956 (PCRE regular expressions with JIT enabled gives different result)
--INI--
pcre.jit=1
--FILE--
<?php
preg_match( '/<(\w+)[\s\w\-]+ id="S44_i89ew">/', '<br><div id="S44_i89ew">', $matches );
var_dump($matches);
?>
--EXPECT--
array(2) {
  [0]=>
  string(20) "<div id="S44_i89ew">"
  [1]=>
  string(2) "di"
}
