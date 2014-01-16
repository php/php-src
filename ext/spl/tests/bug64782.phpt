--TEST--
Bug #64782: SplFileObject constructor make $context optional / give it a default value
--FILE--
<?php

var_dump(new SplFileObject(__FILE__, "r", false, null));

?>
--EXPECTF--
object(SplFileObject)#1 (%d) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s/bug64782.php"
  ["fileName":"SplFileInfo":private]=>
  string(12) "bug64782.php"
  ["openMode":"SplFileObject":private]=>
  string(1) "r"
  ["delimiter":"SplFileObject":private]=>
  string(1) ","
  ["enclosure":"SplFileObject":private]=>
  string(1) """
}
