--TEST--
Test explode() function : usage variations - match longer string
--FILE--
<?php

echo "*** Testing explode() function: match longer string ***\n";

$pizza  = "piece1 piece2 piece3 piece4 piece5 piece6 p";
$pieces = explode(" p", $pizza);
var_dump($pieces);
?>
--EXPECT--
*** Testing explode() function: match longer string ***
array(7) {
  [0]=>
  string(6) "piece1"
  [1]=>
  string(5) "iece2"
  [2]=>
  string(5) "iece3"
  [3]=>
  string(5) "iece4"
  [4]=>
  string(5) "iece5"
  [5]=>
  string(5) "iece6"
  [6]=>
  string(0) ""
}
