--TEST--
sort() on arrays that are not packed lists of integers, or with other flags
--FILE--
<?php
$x = 2;
$inputs = [
    'float'          => [3, 1.5, 2, 1],
    'numeric string' => [10, '9', 1, '1e1'],
    'null and bool'  => [1, null, true, false, 0],
    'string keys'    => ['b' => 2, 'a' => 1, 'c' => 3],
    'reference'      => [3, &$x, 1],
];
foreach ($inputs as $label => $a) {
    sort($a);
    echo "$label: ";
    var_dump($a);
}

echo "The reference survives sorting:\n";
$a = [3, &$x, 1];
sort($a);
$x = 100;
echo json_encode($a), "\n";

echo "SORT_NUMERIC compares as floats, so ties between large integers keep their order:\n";
$a = [2 ** 53 + 1, 2 ** 53, 2 ** 53 + 1, 2 ** 53];
sort($a, SORT_NUMERIC);
echo json_encode($a), "\n";
sort($a);
echo json_encode($a), "\n";

echo "SORT_STRING compares as strings:\n";
$a = [10, 9, 100, -1];
sort($a, SORT_STRING);
echo json_encode($a), "\n";
rsort($a, SORT_STRING);
echo json_encode($a), "\n";
?>
--EXPECT--
float: array(4) {
  [0]=>
  int(1)
  [1]=>
  float(1.5)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
numeric string: array(4) {
  [0]=>
  int(1)
  [1]=>
  string(1) "9"
  [2]=>
  int(10)
  [3]=>
  string(3) "1e1"
}
null and bool: array(5) {
  [0]=>
  NULL
  [1]=>
  bool(false)
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  bool(true)
}
string keys: array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
reference: array(3) {
  [0]=>
  int(1)
  [1]=>
  &int(2)
  [2]=>
  int(3)
}
The reference survives sorting:
[1,100,3]
SORT_NUMERIC compares as floats, so ties between large integers keep their order:
[9007199254740993,9007199254740992,9007199254740993,9007199254740992]
[9007199254740992,9007199254740992,9007199254740993,9007199254740993]
SORT_STRING compares as strings:
[-1,10,100,9]
[9,100,10,-1]
