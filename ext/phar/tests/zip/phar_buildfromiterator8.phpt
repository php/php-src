--TEST--
Phar::buildFromIterator() iterator, SplFileInfo as current zip-based
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--INI--
phar.readonly=0
--FILE--
<?php
try {
    chdir(__DIR__);
    $phar = new Phar(__DIR__ . '/buildfromiterator.phar.zip');
    $a = $phar->buildFromIterator(new RegexIterator(new DirectoryIterator('.'), '/^frontcontroller\d{0,2}\.phar\.phpt\\z|^\.\\z|^\.\.\\z/'), __DIR__ . DIRECTORY_SEPARATOR);
    asort($a);
    var_dump($a);
    var_dump($phar->isFileFormat(Phar::ZIP));
} catch (Exception $e) {
    var_dump(get_class($e));
    echo $e->getMessage() . "\n";
}
?>
--CLEAN--
<?php
unlink(__DIR__ . '/buildfromiterator.phar.zip');
__HALT_COMPILER();
?>
--EXPECTF--
array(21) {
  ["frontcontroller1.phar.phpt"]=>
  string(%d) "%sfrontcontroller1.phar.phpt"
  ["frontcontroller10.phar.phpt"]=>
  string(%d) "%sfrontcontroller10.phar.phpt"
  ["frontcontroller11.phar.phpt"]=>
  string(%d) "%sfrontcontroller11.phar.phpt"
  ["frontcontroller12.phar.phpt"]=>
  string(%d) "%sfrontcontroller12.phar.phpt"
  ["frontcontroller13.phar.phpt"]=>
  string(%d) "%sfrontcontroller13.phar.phpt"
  ["frontcontroller14.phar.phpt"]=>
  string(%d) "%sfrontcontroller14.phar.phpt"
  ["frontcontroller15.phar.phpt"]=>
  string(%d) "%sfrontcontroller15.phar.phpt"
  ["frontcontroller16.phar.phpt"]=>
  string(%d) "%sfrontcontroller16.phar.phpt"
  ["frontcontroller17.phar.phpt"]=>
  string(%d) "%sfrontcontroller17.phar.phpt"
  ["frontcontroller18.phar.phpt"]=>
  string(%d) "%sfrontcontroller18.phar.phpt"
  ["frontcontroller19.phar.phpt"]=>
  string(%d) "%sfrontcontroller19.phar.phpt"
  ["frontcontroller2.phar.phpt"]=>
  string(%d) "%sfrontcontroller2.phar.phpt"
  ["frontcontroller20.phar.phpt"]=>
  string(%d) "%sfrontcontroller20.phar.phpt"
  ["frontcontroller21.phar.phpt"]=>
  string(%d) "%sfrontcontroller21.phar.phpt"
  ["frontcontroller3.phar.phpt"]=>
  string(%d) "%sfrontcontroller3.phar.phpt"
  ["frontcontroller4.phar.phpt"]=>
  string(%d) "%sfrontcontroller4.phar.phpt"
  ["frontcontroller5.phar.phpt"]=>
  string(%d) "%sfrontcontroller5.phar.phpt"
  ["frontcontroller6.phar.phpt"]=>
  string(%d) "%sfrontcontroller6.phar.phpt"
  ["frontcontroller7.phar.phpt"]=>
  string(%d) "%sfrontcontroller7.phar.phpt"
  ["frontcontroller8.phar.phpt"]=>
  string(%d) "%sfrontcontroller8.phar.phpt"
  ["frontcontroller9.phar.phpt"]=>
  string(%d) "%sfrontcontroller9.phar.phpt"
}
bool(true)
