--TEST--
Standard success for SplObjectStorage::offsetGet
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$s = new SplObjectStorage();
$o1 = new stdClass();
$s[$o1] = 'some_value';

echo $s->offsetGet($o1);

?>
--EXPECT--
some_value
