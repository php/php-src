--TEST--
SPL: Test that serialized blob contains unique elements (CVE-2010-2225)
--FILE--
<?php

$badblobs = array(
'x:i:2;i:0;;i:0;;m:a:0:{}',
'x:i:2;O:8:"stdClass":0:{};R:1;;m:a:0:{}',
'x:i:3;O:8:"stdClass":0:{};r:1;;r:1;;m:a:0:{}',
);
foreach($badblobs as $blob) {
try {
  $so = new SplObjectStorage();
  $so->unserialize($blob);
  var_dump($so);
} catch(UnexpectedValueException $e) {
	echo $e->getMessage()."\n";
}
}
--EXPECTF--
Error at offset 6 of 24 bytes
Error at offset 26 of 39 bytes
object(SplObjectStorage)#2 (0) {
}
