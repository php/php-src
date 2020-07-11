--TEST--
ZE2 iterators cannot implement Traversable alone
--FILE--
<?php

class test implements Traversable {
}

$obj = new test;

foreach($obj as $v);

print "Done\n";
/* the error doesn't show the filename but 'Unknown' */
?>
--EXPECTF--
Fatal error: Class test must implement interface Traversable as part of either Iterator or IteratorAggregate in %s on line %d
