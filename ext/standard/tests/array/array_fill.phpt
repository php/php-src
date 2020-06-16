--TEST--
basic array_fill test
--FILE--
<?php
$array1 = array(0.0, 1, 2.5);
$array2 = array(TRUE, FALSE, NULL, "d", "e", "f");
foreach($array1 as $start)
{
   foreach($array1 as $num)
   {
      foreach($array2 as $value)
      {
         echo '==========================='."\n";
     echo 'start: '.$start.' num: '.$num.' value: '.$value."\n";
     $output = array_fill($start, $num, $value);
     var_dump($output);
      }
   }
}
?>
--EXPECT--
===========================
start: 0 num: 0 value: 1
array(0) {
}
===========================
start: 0 num: 0 value: 
array(0) {
}
===========================
start: 0 num: 0 value: 
array(0) {
}
===========================
start: 0 num: 0 value: d
array(0) {
}
===========================
start: 0 num: 0 value: e
array(0) {
}
===========================
start: 0 num: 0 value: f
array(0) {
}
===========================
start: 0 num: 1 value: 1
array(1) {
  [0]=>
  bool(true)
}
===========================
start: 0 num: 1 value: 
array(1) {
  [0]=>
  bool(false)
}
===========================
start: 0 num: 1 value: 
array(1) {
  [0]=>
  NULL
}
===========================
start: 0 num: 1 value: d
array(1) {
  [0]=>
  string(1) "d"
}
===========================
start: 0 num: 1 value: e
array(1) {
  [0]=>
  string(1) "e"
}
===========================
start: 0 num: 1 value: f
array(1) {
  [0]=>
  string(1) "f"
}
===========================
start: 0 num: 2.5 value: 1
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(true)
}
===========================
start: 0 num: 2.5 value: 
array(2) {
  [0]=>
  bool(false)
  [1]=>
  bool(false)
}
===========================
start: 0 num: 2.5 value: 
array(2) {
  [0]=>
  NULL
  [1]=>
  NULL
}
===========================
start: 0 num: 2.5 value: d
array(2) {
  [0]=>
  string(1) "d"
  [1]=>
  string(1) "d"
}
===========================
start: 0 num: 2.5 value: e
array(2) {
  [0]=>
  string(1) "e"
  [1]=>
  string(1) "e"
}
===========================
start: 0 num: 2.5 value: f
array(2) {
  [0]=>
  string(1) "f"
  [1]=>
  string(1) "f"
}
===========================
start: 1 num: 0 value: 1
array(0) {
}
===========================
start: 1 num: 0 value: 
array(0) {
}
===========================
start: 1 num: 0 value: 
array(0) {
}
===========================
start: 1 num: 0 value: d
array(0) {
}
===========================
start: 1 num: 0 value: e
array(0) {
}
===========================
start: 1 num: 0 value: f
array(0) {
}
===========================
start: 1 num: 1 value: 1
array(1) {
  [1]=>
  bool(true)
}
===========================
start: 1 num: 1 value: 
array(1) {
  [1]=>
  bool(false)
}
===========================
start: 1 num: 1 value: 
array(1) {
  [1]=>
  NULL
}
===========================
start: 1 num: 1 value: d
array(1) {
  [1]=>
  string(1) "d"
}
===========================
start: 1 num: 1 value: e
array(1) {
  [1]=>
  string(1) "e"
}
===========================
start: 1 num: 1 value: f
array(1) {
  [1]=>
  string(1) "f"
}
===========================
start: 1 num: 2.5 value: 1
array(2) {
  [1]=>
  bool(true)
  [2]=>
  bool(true)
}
===========================
start: 1 num: 2.5 value: 
array(2) {
  [1]=>
  bool(false)
  [2]=>
  bool(false)
}
===========================
start: 1 num: 2.5 value: 
array(2) {
  [1]=>
  NULL
  [2]=>
  NULL
}
===========================
start: 1 num: 2.5 value: d
array(2) {
  [1]=>
  string(1) "d"
  [2]=>
  string(1) "d"
}
===========================
start: 1 num: 2.5 value: e
array(2) {
  [1]=>
  string(1) "e"
  [2]=>
  string(1) "e"
}
===========================
start: 1 num: 2.5 value: f
array(2) {
  [1]=>
  string(1) "f"
  [2]=>
  string(1) "f"
}
===========================
start: 2.5 num: 0 value: 1
array(0) {
}
===========================
start: 2.5 num: 0 value: 
array(0) {
}
===========================
start: 2.5 num: 0 value: 
array(0) {
}
===========================
start: 2.5 num: 0 value: d
array(0) {
}
===========================
start: 2.5 num: 0 value: e
array(0) {
}
===========================
start: 2.5 num: 0 value: f
array(0) {
}
===========================
start: 2.5 num: 1 value: 1
array(1) {
  [2]=>
  bool(true)
}
===========================
start: 2.5 num: 1 value: 
array(1) {
  [2]=>
  bool(false)
}
===========================
start: 2.5 num: 1 value: 
array(1) {
  [2]=>
  NULL
}
===========================
start: 2.5 num: 1 value: d
array(1) {
  [2]=>
  string(1) "d"
}
===========================
start: 2.5 num: 1 value: e
array(1) {
  [2]=>
  string(1) "e"
}
===========================
start: 2.5 num: 1 value: f
array(1) {
  [2]=>
  string(1) "f"
}
===========================
start: 2.5 num: 2.5 value: 1
array(2) {
  [2]=>
  bool(true)
  [3]=>
  bool(true)
}
===========================
start: 2.5 num: 2.5 value: 
array(2) {
  [2]=>
  bool(false)
  [3]=>
  bool(false)
}
===========================
start: 2.5 num: 2.5 value: 
array(2) {
  [2]=>
  NULL
  [3]=>
  NULL
}
===========================
start: 2.5 num: 2.5 value: d
array(2) {
  [2]=>
  string(1) "d"
  [3]=>
  string(1) "d"
}
===========================
start: 2.5 num: 2.5 value: e
array(2) {
  [2]=>
  string(1) "e"
  [3]=>
  string(1) "e"
}
===========================
start: 2.5 num: 2.5 value: f
array(2) {
  [2]=>
  string(1) "f"
  [3]=>
  string(1) "f"
}
