--TEST--
SPL: SplPriorityQueue: top and extract flags
--CREDITS--
Nathaniel McHugh nat@fishtrap.co.uk
#testfest London 2009-05-09
--FILE--
<?php

$priorityQueue = new SplPriorityQueue();

var_dump($priorityQueue->getExtractFlags());

$priorityQueue->insert("a", 1);
$priorityQueue->insert("b", 2);
$priorityQueue->insert("c", 0);

echo "EXTR DEFAULT",PHP_EOL;
echo "value: ",$priorityQueue->top(),PHP_EOL;

$priorityQueue->setExtractFlags(SplPriorityQueue::EXTR_PRIORITY);
var_dump($priorityQueue->getExtractFlags() & SplPriorityQueue::EXTR_PRIORITY);

echo "EXTR_PRIORITY",PHP_EOL;
echo "priority: ",$priorityQueue->top(),PHP_EOL;

$priorityQueue->setExtractFlags(SplPriorityQueue::EXTR_BOTH);
echo "EXTR_BOTH",PHP_EOL;
print_r($priorityQueue->top());

echo "EXTR_DATA",PHP_EOL;
$priorityQueue->setExtractFlags(SplPriorityQueue::EXTR_DATA);
echo "value: ",$priorityQueue->top(),PHP_EOL;
?>
--EXPECT--
int(1)
EXTR DEFAULT
value: b
int(2)
EXTR_PRIORITY
priority: 2
EXTR_BOTH
Array
(
    [data] => b
    [priority] => 2
)
EXTR_DATA
value: b