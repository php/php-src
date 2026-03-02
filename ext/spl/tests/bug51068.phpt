--TEST--
SPL: glob wrapper interactions with DirectoryIterator
--FILE--
<?php
touch('bug.51068');
mkdir('bug.51068.dir');
touch('bug.51068.dir/lvl2.bug.51068');
$iter = new DirectoryIterator('glob://*.51068');
foreach ($iter as $f) {
    var_dump($f->getFilename());
    var_dump($f->getSize());
}
$iter = new DirectoryIterator('glob://bug.51068.dir/*.51068');
foreach ($iter as $f) {
  var_dump($f->getFilename());
  var_dump($f->getSize());
}
$iter = new DirectoryIterator('glob://bug.51068.dir');
foreach ($iter as $f) {
  var_dump($f->getFilename());
  var_dump($f->getSize() >= 0);
}
?>
--CLEAN--
<?php
unlink('bug.51068');
unlink('bug.51068.dir/lvl2.bug.51068');
rmdir('bug.51068.dir');
?>
--EXPECT--
string(9) "bug.51068"
int(0)
string(14) "lvl2.bug.51068"
int(0)
string(13) "bug.51068.dir"
bool(true)
