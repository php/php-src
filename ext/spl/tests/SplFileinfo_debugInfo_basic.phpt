--TEST--
SPL: SplFileInfo::_debugInfo() basic test
--FILE--
<?php
// without $suffix
var_dump(new \SplFileInfo('/path/to/a.txt'));
var_dump(new \SplFileInfo('path/to/b'));
var_dump(new \SplFileInfo('c.txt'));
var_dump(new \SplFileInfo('d'));
var_dump(new \SplFileInfo('~/path/to//e'));

// with $suffix
var_dump(new \SplFileInfo('path/to/a.txt'));
var_dump(new \SplFileInfo('path/to/bbb.txt'));
var_dump(new \SplFileInfo('path/to/ccc.txt'));
var_dump(new \SplFileInfo('d.txt'));
var_dump(new \SplFileInfo('e.txt'));
var_dump(new \SplFileInfo('f'));
?>
--EXPECT--
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(14) "/path/to/a.txt"
  ["fileName":"SplFileInfo":private]=>
  string(5) "a.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(9) "path/to/b"
  ["fileName":"SplFileInfo":private]=>
  string(1) "b"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(5) "c.txt"
  ["fileName":"SplFileInfo":private]=>
  string(5) "c.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(1) "d"
  ["fileName":"SplFileInfo":private]=>
  string(1) "d"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(12) "~/path/to//e"
  ["fileName":"SplFileInfo":private]=>
  string(1) "e"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(13) "path/to/a.txt"
  ["fileName":"SplFileInfo":private]=>
  string(5) "a.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(15) "path/to/bbb.txt"
  ["fileName":"SplFileInfo":private]=>
  string(7) "bbb.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(15) "path/to/ccc.txt"
  ["fileName":"SplFileInfo":private]=>
  string(7) "ccc.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(5) "d.txt"
  ["fileName":"SplFileInfo":private]=>
  string(5) "d.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(5) "e.txt"
  ["fileName":"SplFileInfo":private]=>
  string(5) "e.txt"
}
object(SplFileInfo)#1 (2) {
  ["pathName":"SplFileInfo":private]=>
  string(1) "f"
  ["fileName":"SplFileInfo":private]=>
  string(1) "f"
}
