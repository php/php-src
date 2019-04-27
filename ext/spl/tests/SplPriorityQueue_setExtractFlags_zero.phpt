--TEST--
Setting SplPriorityQueue extract flags to zero generates an exception
--FILE--
<?php

$queue = new SplPriorityQueue();
$queue->setExtractFlags(0);

?>
--EXPECTF--
Fatal error: Uncaught RuntimeException: Must specify at least one extract flag in %s:%d
Stack trace:
#0 %s(%d): SplPriorityQueue->setExtractFlags(0)
#1 {main}
  thrown in %s on line %d
