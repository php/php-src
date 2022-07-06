--TEST--
SPL SplTempFileObject constructor sets correct values when passed fixed memory size
--FILE--
<?php
var_dump(new SplTempFileObject(1024));
?>
--EXPECT--
object(SplTempFileObject)#1 (5) {
  ["pathName":"SplFileInfo":private]=>
  string(25) "php://temp/maxmemory:1024"
  ["fileName":"SplFileInfo":private]=>
  string(25) "php://temp/maxmemory:1024"
  ["openMode":"SplFileObject":private]=>
  string(1) "w"
  ["delimiter":"SplFileObject":private]=>
  string(1) ","
  ["enclosure":"SplFileObject":private]=>
  string(1) """
}
