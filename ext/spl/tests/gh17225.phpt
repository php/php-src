--TEST--
GH-17225 (NULL deref in spl_directory.c)
--CREDITS--
YuanchengJiang
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/gh17225.phar.zip';
$phar = new Phar($fname);
class HasDestructor {
    public function __destruct() {
        var_dump($GLOBALS['s']);
    }
}
$s = new SplObjectStorage();
$s[$phar] = new HasDestructor();
register_shutdown_function(function() {
    global $s;
});
var_dump($phar->isLink());
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh17225.phar.zip');
?>
--EXPECTF--
bool(false)
object(SplObjectStorage)#%d (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(Phar)#%d (4) {
        ["pathName":"SplFileInfo":private]=>
        string(0) ""
        ["fileName":"SplFileInfo":private]=>
        string(0) ""
        ["glob":"DirectoryIterator":private]=>
        bool(false)
        ["subPathName":"RecursiveDirectoryIterator":private]=>
        string(0) ""
      }
      ["inf"]=>
      object(HasDestructor)#%d (0) {
      }
    }
  }
}
