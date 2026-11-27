--TEST--
Test Uri\QueryParams::sortByUtf16() - UTF-16 code unit order across the surrogate range
--FILE--
<?php

$params1 = (new Uri\QueryParams())
    ->append("p\u{FF5E}", "tilde")
    ->append("p\u{1F600}a", "prefix")
    ->append("p\u{10FFFF}", "last")
    ->append("p\u{E000}", "private")
    ->append("p\u{1F601}", "grin")
    ->append("p\u{10000}", "first")
    ->append("p\u{D7FF}", "before")
    ->append("p\u{1F600}", "smile")
    ->append("p", "ascii");
$params2 = $params1->sortByUtf16();

var_dump(array_column($params2->toList(), 1));

?>
--EXPECT--
array(9) {
  [0]=>
  string(5) "ascii"
  [1]=>
  string(6) "before"
  [2]=>
  string(5) "first"
  [3]=>
  string(5) "smile"
  [4]=>
  string(6) "prefix"
  [5]=>
  string(4) "grin"
  [6]=>
  string(4) "last"
  [7]=>
  string(7) "private"
  [8]=>
  string(5) "tilde"
}
