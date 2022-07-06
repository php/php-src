--TEST--
SPL: SplHeap: comparison callback
--FILE--
<?php
class myHeap extends SplHeap {
    public function compare($a, $b) {
        if ($a > $b) {
            $result = 1;
        } else if ($a < $b) {
            $result = -1;
        } else {
            $result = 0;
        }
        return $result;
    }
}

$h = new myHeap;

$in = range(0,10);
shuffle($in);
foreach ($in as $i) {
    $h->insert($i);
}

foreach ($h as $out) {
    echo $out."\n";
}
?>
--EXPECT--
10
9
8
7
6
5
4
3
2
1
0
