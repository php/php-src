--TEST--
Testing randomization of shuffle() and str_shuffle().
--FILE--
<?php
function stats($f, $a) {
    $times = 90000;
    print "$f\n";
    ksort($a);
    foreach($a as $k => $v)
        print "$k: $v: " . sprintf('%0.3f', $v / $times) . "\n";
}
$a = array();
$times = 90000;
for ($i = 0; $i < $times; $i++) {
    $p = range(1,4);
    shuffle($p);
    $s = join('', $p);
    if (empty($a[$s])) $a[$s] = 0; 
    $a[$s]++;
}

stats('shuffle', $a);
$a = array();
$times = 90000;
for ($i = 0; $i < $times; $i++) {
    $p = '1234';
    $s = str_shuffle($p);
    if (empty($a[$s])) $a[$s] = 0;
    $a[$s]++;
}

stats('str_shuffle', $a);
?>
--EXPECTF--
shuffle
1234: 3%d: 0.04%d
1243: 3%d: 0.04%d
1324: 3%d: 0.04%d
1342: 3%d: 0.04%d
1423: 3%d: 0.04%d
1432: 3%d: 0.04%d
2134: 3%d: 0.04%d
2143: 3%d: 0.04%d
2314: 3%d: 0.04%d
2341: 3%d: 0.04%d
2413: 3%d: 0.04%d
2431: 3%d: 0.04%d
3124: 3%d: 0.04%d
3142: 3%d: 0.04%d
3214: 3%d: 0.04%d
3241: 3%d: 0.04%d
3412: 3%d: 0.04%d
3421: 3%d: 0.04%d
4123: 3%d: 0.04%d
4132: 3%d: 0.04%d
4213: 3%d: 0.04%d
4231: 3%d: 0.04%d
4312: 3%d: 0.04%d
4321: 3%d: 0.04%d
str_shuffle
1234: 3%d: 0.04%d
1243: 3%d: 0.04%d
1324: 3%d: 0.04%d
1342: 3%d: 0.04%d
1423: 3%d: 0.04%d
1432: 3%d: 0.04%d
2134: 3%d: 0.04%d
2143: 3%d: 0.04%d
2314: 3%d: 0.04%d
2341: 3%d: 0.04%d
2413: 3%d: 0.04%d
2431: 3%d: 0.04%d
3124: 3%d: 0.04%d
3142: 3%d: 0.04%d
3214: 3%d: 0.04%d
3241: 3%d: 0.04%d
3412: 3%d: 0.04%d
3421: 3%d: 0.04%d
4123: 3%d: 0.04%d
4132: 3%d: 0.04%d
4213: 3%d: 0.04%d
4231: 3%d: 0.04%d
4312: 3%d: 0.04%d
4321: 3%d: 0.04%d
