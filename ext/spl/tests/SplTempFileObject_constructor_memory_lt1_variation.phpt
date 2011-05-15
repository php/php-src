--TEST--
SPL SplTempFileObject constructor sets correct defaults when passed a negative value
--FILE--
<?php
var_dump(new SplTempFileObject(-1));
?>
--EXPECTF--
object(SplTempFileObject)#1 (5) {
  ["pathName":"SplFileInfo":private]=>
  string(12) "php://memory"
  ["fileName":"SplFileInfo":private]=>
  string(12) "php://memory"
  ["openMode":"SplFileObject":private]=>
  string(1) "w"
  ["delimiter":"SplFileObject":private]=>
  string(1) ","
  ["enclosure":"SplFileObject":private]=>
  string(1) """
}
