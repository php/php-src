--TEST--
SPL: SplMaxHeap: std operations
--FILE--
<?php
$h = new SplMaxHeap();

// errors
try {
    $h->extract();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}


$h->insert(1);
$h->insert(2);
$h->insert(3);
$h->insert(3);
$h->insert(3);

echo $h->count()."\n";
echo $h->extract()."\n";
echo $h->extract()."\n";
echo $h->extract()."\n";
echo $h->extract()."\n";
echo $h->extract()."\n";
echo $h->count()."\n";

echo "--\n";

$b = 4;
$h->insert($b);
$b = 5;

$h2 = clone $h;
echo $h->extract()."\n";
echo $h2->extract()."\n";
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Exception: Can't extract from an empty heap
5
3
3
3
2
1
0
--
4
4
===DONE===
