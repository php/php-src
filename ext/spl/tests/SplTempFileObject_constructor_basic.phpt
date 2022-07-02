--TEST--
SPL SplTempFileObject constructor sets correct defaults when pass 0 arguments
--FILE--
<?php
var_dump(new SplTempFileObject());
?>
--EXPECT--
object(SplTempFileObject)#1 (5) {
  ["pathName":"SplFileInfo":private]=>
  string(10) "php://temp"
  ["fileName":"SplFileInfo":private]=>
  string(10) "php://temp"
  ["openMode":"SplFileObject":private]=>
  string(2) "wb"
  ["delimiter":"SplFileObject":private]=>
  string(1) ","
  ["enclosure":"SplFileObject":private]=>
  string(1) """
}
