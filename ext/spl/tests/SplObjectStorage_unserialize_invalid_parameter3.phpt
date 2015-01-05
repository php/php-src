--TEST--
Check that SplObjectStorage::unserialize doesn't throws exception when NULL passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

try {
    $s->unserialize(NULL);
} catch(UnexpectedValueException $e) {
    echo $e->getMessage();
}

?>
Done
--EXPECTF--
Done
