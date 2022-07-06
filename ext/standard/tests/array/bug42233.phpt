--TEST--
Bug #42233 (extract(): scandic characters not allowed as variable name)
--FILE--
<?php

$test = array(
    'a'     => '1',
    'æ'     => '2',
    'æøåäö' => '3',
);

var_dump($test);
var_dump(extract($test));
var_dump($a);
var_dump($æ);
var_dump($æøåäö);

echo "Done.\n";
?>
--EXPECT--
array(3) {
  ["a"]=>
  string(1) "1"
  ["æ"]=>
  string(1) "2"
  ["æøåäö"]=>
  string(1) "3"
}
int(3)
string(1) "1"
string(1) "2"
string(1) "3"
Done.
