--TEST--
Setting SplPriorityQueue extract flags to zero generates an exception
--FILE--
<?php

$queue = new SplPriorityQueue();
try {
    $queue->setExtractFlags(0);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
SplPriorityQueue::setExtractFlags(): Argument #1 ($flags) must specify at least one extract flag
