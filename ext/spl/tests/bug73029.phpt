--TEST--
Bug #73029: Missing type check when unserializing SplArray
--FILE--
<?php
try {
$a = 'C:11:"ArrayObject":19:{x:i:0;r:2;;m:a:0:{}}';
$m = unserialize($a);
$x = $m[2];
} catch(UnexpectedValueException $e) {
    print $e->getMessage() . "\n";
}
try {
$a = 'C:11:"ArrayObject":19:0x:i:0;r:2;;m:a:0:{}}';
$m = unserialize($a);
$x = $m[2];
} catch(UnexpectedValueException $e) {
    print $e->getMessage() . "\n";
}
?>
DONE
--EXPECTF--
Error at offset 10 of 19 bytes

Warning: unserialize(): Error at offset 22 of 43 bytes in %s on line %d

Warning: Trying to access array offset on false in %s on line %d
DONE
