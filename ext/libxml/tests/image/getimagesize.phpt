--TEST--
getimagesize() with svg input
--EXTENSIONS--
libxml
--FILE--
<?php

$inputs = [
    "<?xml version=\"1.0\"?>",
    "svg width=\"1\" height=\"1\"",
    <<<XML
<?xml version="1.0" standalone="yes"?>
<!-- foo <svg width="1" height="1"/> -->
<x:svg width='4cm' height="8cm" xmlns:x="http://www.w3.org/2000/svg">
XML,
    "<SVG width='1' height=\"1\"/>",
    "<SVG width='1px' height=\"1\"/>",
    "<SVG width='1' height=\"1mm\"/>",
    "<SVG width='1' height=\"cm\"/>",
    "<SVG width='' height=\"\"/>",
    "<SVG width=\"é\" height='1'/>",
    "<foo width='1' height=\"1\"/>",
    "<foo foo='1' height=\"1\"/>",
];

foreach ($inputs as $input) {
    var_dump(getimagesizefromstring($input));
}

?>
--EXPECTF--
bool(false)
bool(false)
array(6) {
  [0]=>
  int(4)
  [1]=>
  int(8)
  [2]=>
  int(%d)
  ["mime"]=>
  string(13) "image/svg+xml"
  ["width_unit"]=>
  string(2) "cm"
  ["height_unit"]=>
  string(2) "cm"
}
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(%d)
  [3]=>
  string(20) "width="1" height="1""
  ["mime"]=>
  string(13) "image/svg+xml"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(%d)
  [3]=>
  string(20) "width="1" height="1""
  ["mime"]=>
  string(13) "image/svg+xml"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "px"
}
array(6) {
  [0]=>
  int(1)
  [1]=>
  int(1)
  [2]=>
  int(%d)
  ["mime"]=>
  string(13) "image/svg+xml"
  ["width_unit"]=>
  string(2) "px"
  ["height_unit"]=>
  string(2) "mm"
}
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
