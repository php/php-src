--TEST--
GH-12826 (Weird pointers issue in nested loops)
--FILE--
<?php
$test = array(
  'a' => 1,
  'b' => 2,
  'c' => 3,
  'd' => 4,
);

unset($test['a']);
unset($test['b']);

foreach($test as $k => &$v) { // Mind the reference!
    echo "Pass $k : ";

    foreach($test as $kk => $vv) {
        echo $test[$kk];
        if ($kk == $k) $test[$kk] = 0;
    }

    echo "\n";
}

unset($v);
?>
--EXPECT--
Pass c : 34
Pass d : 04
