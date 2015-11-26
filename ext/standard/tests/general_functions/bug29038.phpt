--TEST--
Bug #29038 (extract(), EXTR_PREFIX_SAME option prefixes empty strings)
--FILE--
<?php
function my_print_r($a) {
	ksort($a);
	print_r($a);
}
function f1() {
  $c = extract(array("" => 1),EXTR_PREFIX_SAME,"prefix");
  echo "Extracted:";
  var_dump($c);
  my_print_r(get_defined_vars());
}
function f2() {
  $a = 1;
  $c = extract(array("a" => 1),EXTR_PREFIX_SAME,"prefix");
  echo "Extracted:";
  var_dump($c);
  my_print_r(get_defined_vars());
}
function f3() {
  $a = 1;
  $c = extract(array("a" => 1),EXTR_PREFIX_ALL,"prefix");
  echo "Extracted:";
  var_dump($c);
  my_print_r(get_defined_vars());
}
function f4() {
  $c = extract(array("" => 1),EXTR_PREFIX_ALL,"prefix");
  echo "Extracted:";
  var_dump($c);
  my_print_r(get_defined_vars());
}
function f5() {
  $c = extract(array("111" => 1),EXTR_PREFIX_ALL,"prefix");
  echo "Extracted:";
  var_dump($c);
  my_print_r(get_defined_vars());
}

f1();
f2();
f3();
f4();
f5();
?>
--EXPECT--
Extracted:int(0)
Array
(
    [c] => 0
)
Extracted:int(1)
Array
(
    [a] => 1
    [c] => 1
    [prefix_a] => 1
)
Extracted:int(1)
Array
(
    [a] => 1
    [c] => 1
    [prefix_a] => 1
)
Extracted:int(0)
Array
(
    [c] => 0
)
Extracted:int(1)
Array
(
    [c] => 1
    [prefix_111] => 1
)
