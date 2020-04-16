--TEST--
SPL: SplFileInfo cloning
--FILE--
<?php

function test($name, $lc, $lp)
{
    static $i = 0;
    echo "===$i===\n";
    $i++;

    $o = new SplFileInfo($name);

    var_dump($o);
    $c = clone $o;
    var_dump($c);
    var_dump($o === $c);
    var_dump($o == $c);
    var_dump($o->getPathname() == $c->getPathname());

    try {
        $f = new SplFileObject($name);
        var_dump($name);
        var_dump($f->getPathName());
        $l = substr($f->getPathName(), -1);
        var_dump($l != '/' && $l != '\\' && $l == $lc);
        var_dump($f->getFileName());
        $l = substr($f->getFileName(), -1);
        var_dump($l != '/' && $l != '\\' && $l == $lc);
        var_dump($f->getPath());
        $l = substr($f->getPath(), -1);
        var_dump($l != '/' && $l != '\\' && $l == $lp);
    } catch (\Error $e) {
        echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
    }
    try {
        $fo = $o->openFile();
        var_dump($fo->getPathName(), $fo->getFileName(), $fo->getPath());
    } catch (\Error $e) {
        echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
    }
}

test(__DIR__ . '/' . 'fileobject_001a.txt', 't', substr(__DIR__,-1));
test(__DIR__ . '/', substr(__DIR__,-1), 'l');
test(__DIR__,       substr(__DIR__,-1), 'l');

?>
--EXPECTF--
===0===
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "fileobject_001a.txt"
}
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "fileobject_001a.txt"
}
bool(false)
bool(true)
bool(true)
%s(%d) "%sfileobject_001a.txt"
string(%d) "%sfileobject_001a.txt"
bool(true)
string(19) "fileobject_001a.txt"
bool(true)
string(%d) "%stests"
bool(true)
string(%d) "%sfileobject_001a.txt"
string(19) "fileobject_001a.txt"
string(%d) "%stests"
===1===
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "%s"
}
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "%s"
}
bool(false)
bool(true)
bool(true)
Error: Cannot use SplFileObject with directories
Error: Cannot use SplFileObject with directories
===2===
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "%s"
}
object(SplFileInfo)#%d (2) {
  ["pathName":"SplFileInfo":private]=>
  string(%d) "%s"
  ["fileName":"SplFileInfo":private]=>
  string(%d) "%s"
}
bool(false)
bool(true)
bool(true)
Error: Cannot use SplFileObject with directories
Error: Cannot use SplFileObject with directories
