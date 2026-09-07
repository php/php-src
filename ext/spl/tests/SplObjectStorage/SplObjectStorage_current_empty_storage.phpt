--TEST--
Check that SplObjectStorage::current() throws when iterator invalid
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

var_dump($s->valid());
try {
    var_dump($s->current());
} catch (RuntimeException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
bool(false)
RuntimeException: Called current() on invalid iterator
