--TEST--
Bug #61418: Segmentation fault using FiltesystemIterator & RegexIterator
--FILE--
<?php
$fileIterator = new FilesystemIterator(__DIR__, FilesystemIterator::KEY_AS_FILENAME);
$regexpIterator = new RegexIterator($fileIterator, '#.*#');
foreach ($fileIterator as $key => $file)
{
}
unset($regexpIterator);
unset($fileIterator);

$dirIterator = new DirectoryIterator(__DIR__);
$regexpIterator2 = new RegexIterator($dirIterator, '#.*#');
foreach ($dirIterator as $key => $file)
{
}
unset($regexpIterator2);
unset($dirIterator);
?>
==DONE==
--EXPECT--
==DONE==
