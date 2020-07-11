--TEST--
SPL: SplMinHeap: std operations
--FILE--
<?php
$h = new SplMinHeap();

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

echo $h->extract()."\n";
?>
--EXPECT--
Exception: Can't extract from an empty heap
5
1
2
3
3
3
0
--
4
