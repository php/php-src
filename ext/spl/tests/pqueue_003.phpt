--TEST--
SPL: SplPriorityQueue: iteration through methods
--FILE--
<?php
$h = new SplPriorityQueue();

$h->insert(1, 1);
$h->insert(5, 5);
$h->insert(0, 0);
$h->insert(4, 4);

$h->rewind();
echo "count(\$h) = ".count($h)."\n";
echo "\$h->count() = ".$h->count()."\n";
while ($h->valid()) {
    $k = $h->key();
    $v = $h->current();
    echo "$k=>$v\n";
    $h->next();
}
?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
count($h) = 4
$h->count() = 4
3=>5
2=>4
1=>1
0=>0
===DONE===
