--TEST--
SPL: SplPriorityQueue: std operations and extract flags
--FILE--
<?php
$pq = new SplPriorityQueue();

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

$pq1->insert("a", 1);
$pq1->insert("b", 2);
$pq1->insert("c", 0);

foreach ($pq1 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

echo "EXTR_DATA\n";

$pq2 = new SplPriorityQueue();
$pq2->setExtractFlags(SplPriorityQueue::EXTR_DATA);

$pq2->insert("a", 1);
$pq2->insert("b", 2);
$pq2->insert("c", 0);

foreach ($pq2 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

echo "EXTR_PRIORITY\n";

$pq3 = new SplPriorityQueue();
$pq3->setExtractFlags(SplPriorityQueue::EXTR_PRIORITY);

$pq3->insert("a", 1);
$pq3->insert("b", 2);
$pq3->insert("c", 0);

foreach ($pq3 as $k=>$v) {
    echo "$k=>".print_r($v, 1)."\n";
}

?>
--EXPECT--
Exception: Can't extract from an empty heap
2=>b
1=>a
0=>c
EXTR_BOTH
2=>Array
(
    [data] => b
    [priority] => 2
)

1=>Array
(
    [data] => a
    [priority] => 1
)

0=>Array
(
    [data] => c
    [priority] => 0
)

EXTR_DATA
2=>b
1=>a
0=>c
EXTR_PRIORITY
2=>2
1=>1
0=>0
