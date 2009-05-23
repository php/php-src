--TEST--
Check that SplObjectStorage::unserialize throws exception when NULL passed
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
--EXPECTF--
Empty serialized string cannot be empty

