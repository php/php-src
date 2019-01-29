--TEST--
SPL: glob wrapper interactions with DirectoryIterator
--FILE--
<?php
touch('bug.51068');
$iter = new DirectoryIterator('glob://*.51068');
foreach ($iter as $f) {
	var_dump($f->getFilename());
	var_dump($f->getSize());
}
?>
--CLEAN--
<?php
unlink('bug.51068');
?>
--EXPECT--
string(9) "bug.51068"
int(0)
