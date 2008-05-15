--TEST--
SPL: SplPriorityQueue: std operations and extract flags
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip"; ?>
--FILE--
<?php
$pq = new SplPriorityQueue();
echo $pq->getExtractFlags()."\n";

// errors
try {
    $pq->extract();
} catch (RuntimeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

$pq->insert("a", 1);
$pq->insert("b", 2);
$pq->insert("c", 0);

foreach ($pq as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

echo "EXTR_BOTH\n";

$pq1 = new SplPriorityQueue();
$pq1->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
echo $pq1->getExtractFlags()."\n";

$pq1->insert("a", 1);
$pq1->insert("b", 2);
$pq1->insert("c", 0);

foreach ($pq1 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

echo "EXTR_DATA\n";

$pq2 = new SplPriorityQueue();
$pq2->setExtractFlags(SplPriorityQueue::EXTR_DATA);
echo $pq2->getExtractFlags()."\n";

$pq2->insert("a", 1);
$pq2->insert("b", 2);
$pq2->insert("c", 0);

foreach ($pq2 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

echo "EXTR_PRIORITY\n";

$pq3 = new SplPriorityQueue();
$pq3->setExtractFlags(SplPriorityQueue::EXTR_PRIORITY);
echo $pq3->getExtractFlags()."\n";

$pq3->insert("a", 1);
$pq3->insert("b", 2);
$pq3->insert("c", 0);

foreach ($pq3 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
1
Exception: Can't extract from an empty heap
3=>b
2=>a
1=>c
EXTR_BOTH
3
3=>Array
(
    [data] => b
    [priority] => 2
)

2=>Array
(
    [data] => a
    [priority] => 1
)

1=>Array
(
    [data] => c
    [priority] => 0
)

EXTR_DATA
1
3=>b
2=>a
1=>c
EXTR_PRIORITY
2
3=>2
2=>1
1=>0
===DONE===
