--TEST--
Bug #81011 (mb_convert_encoding removes references from arrays)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die("skip mbstring extension not available");
?>
--FILE--
<?php
$array = [
    'ads' => [
        123 => ['name' => 'this', 'id' => 444],
        234 => ['name' => 'that', 'id' => 555],
    ],
    'other' => ['one', 'two']
    ];
         
// we modify array elements using reference     
foreach( $array['ads'] as &$ad ){
    $ad['premium'] = (int)($ad['id'] == 555);
}

var_dump($array);
var_dump(mb_convert_encoding($array, 'UTF-8', 'UTF-8'));
?>
--EXPECT--
array(2) {
  ["ads"]=>
  array(2) {
    [123]=>
    array(3) {
      ["name"]=>
      string(4) "this"
      ["id"]=>
      int(444)
      ["premium"]=>
      int(0)
    }
    [234]=>
    &array(3) {
      ["name"]=>
      string(4) "that"
      ["id"]=>
      int(555)
      ["premium"]=>
      int(1)
    }
  }
  ["other"]=>
  array(2) {
    [0]=>
    string(3) "one"
    [1]=>
    string(3) "two"
  }
}
array(2) {
  ["ads"]=>
  array(2) {
    [123]=>
    array(3) {
      ["name"]=>
      string(4) "this"
      ["id"]=>
      int(444)
      ["premium"]=>
      int(0)
    }
    [234]=>
    array(3) {
      ["name"]=>
      string(4) "that"
      ["id"]=>
      int(555)
      ["premium"]=>
      int(1)
    }
  }
  ["other"]=>
  array(2) {
    [0]=>
    string(3) "one"
    [1]=>
    string(3) "two"
  }
}
