--TEST--
Bug #76505 (array_merge_recursive() is duplicating sub-array keys)
--FILE--
<?php
$array1 = array(
    'k' => array(
        2 => 100,
        98 => 200,
    )
);

$array2 = array(
    'k' => array(
        64 => 300
    )
);

$array3 = array_merge_recursive( $array1, $array2 );

var_dump($array3);
?>
--EXPECT--
array(1) {
  ["k"]=>
  array(3) {
    [2]=>
    int(100)
    [98]=>
    int(200)
    [99]=>
    int(300)
  }
}
