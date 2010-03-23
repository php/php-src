--TEST--
Bug #36306 (crc32() 64bit)
--FILE--
<?php

/* as an example how to write crc32 tests
   PHP does not have uint values, you cannot
   display crc32 like a signed integer.
   Have to find some small strings to truely reproduce 
   the problem, this example being not a problem
*/
echo dechex(crc32("platform independant")) . "\n";
?>
--EXPECT--
ccd9fe66
