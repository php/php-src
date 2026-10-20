--TEST--
Bug #36975 (natcasesort() causes array_pop() to misbehave)
--FILE--
<?php
$a = array('aa', 'aa', 'bb', 'bb', 'cc', 'cc');
$test = natcasesort($a);
if ($test) {
  echo "natcasesort success!\n";
}
$val = array_pop($a);
$a[] = $val;
var_dump($a);

$b = array(1 => 'foo', 0 => 'baz');
array_pop($b);
$b[] = 'bar';
array_push($b, 'bar');
print_r($b);

$c = array(0, 0, 0, 0, 0);
asort($c);
array_pop($c);
$c[] = 'foo';
$c[] = 'bar';
var_dump($c);
?>
--EXPECT--
natcasesort success!
array(6) {
  [0]=>
  string(2) "aa"
  [1]=>
  string(2) "aa"
  [2]=>
  string(2) "bb"
  [3]=>
  string(2) "bb"
  [4]=>
  string(2) "cc"
  [5]=>
  string(2) "cc"
}
Array
(
    [1] => foo
    [2] => bar
    [3] => bar
)
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  string(3) "foo"
  [5]=>
  string(3) "bar"
}
