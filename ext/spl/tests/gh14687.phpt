--TEST--
GH-14687 segfault on debugging SplObjectStorage instance after __destruct.
--CREDITS--
YuanchengJiang
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
phar.readonly=0
--FILE--
<?php
$fname = __DIR__ . '/gh14687.phar.zip';
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
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh14687.phar.zip');
?>
--EXPECT--
object(SplObjectStorage)#2 (1) {
  ["storage":"SplObjectStorage":private]=>
  array(1) {
    [0]=>
    array(2) {
      ["obj"]=>
      object(Phar)#1 (3) {
        ["pathName":"SplFileInfo":private]=>
        string(0) ""
        ["glob":"DirectoryIterator":private]=>
        bool(false)
        ["subPathName":"RecursiveDirectoryIterator":private]=>
        string(0) ""
      }
      ["inf"]=>
      object(HasDestructor)#3 (0) {
      }
    }
  }
}
