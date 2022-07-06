--TEST--
Bug #71660 (array_column behaves incorrectly after foreach by reference)
--FILE--
<?php
$arr = array('id' => 12345, 'name' => 'sam');
foreach ($arr as &$v) {
        $v = $v;
}

$arr = [$arr];

var_dump(array_column($arr, 'name', 'id'));
?>
--EXPECT--
array(1) {
  [12345]=>
  string(3) "sam"
}
