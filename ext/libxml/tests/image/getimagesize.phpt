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
array(5) {
  [0]=>
  string(3) "4cm"
  [1]=>
  string(3) "8cm"
  [2]=>
  int(%d)
  [3]=>
  string(24) "width="4cm" height="8cm""
  ["mime"]=>
  string(13) "image/svg+xml"
}
array(5) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
  [2]=>
  int(%d)
  [3]=>
  string(20) "width="1" height="1""
  ["mime"]=>
  string(13) "image/svg+xml"
}
bool(false)
bool(false)
bool(false)
bool(false)
