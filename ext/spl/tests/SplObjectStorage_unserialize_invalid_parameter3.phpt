--TEST--
Check that SplObjectStorage::unserialize doesn't throws exception when empty string passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();

try {
    $s->unserialize('');
} catch(UnexpectedValueException $e) {
    echo $e->getMessage();
}

?>
Done
--EXPECT--
Done
