--TEST--
SPL: Spl File Info test getLinkTarget
--CREDITS--
Nataniel McHugh nat@fishtrap.co.uk
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') die("skip this test not for Windows platforms");
?>
--FILE--
<?php
$link = 'test_link';
symlink(__FILE__, $link );
$fileInfo = new SplFileInfo($link);

if ($fileInfo->isLink()) {
	echo $fileInfo->getLinkTarget() == __FILE__ ? 'same' : 'different',PHP_EOL;
}
var_dump(unlink($link));
?>
--EXPECT--
same
bool(true)
