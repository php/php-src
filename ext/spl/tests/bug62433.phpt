--TEST--
Bug #62433 Inconsistent behavior of RecursiveDirectoryIterator to dot files (. and ..)
--FILE--
<?php
$dots = array_keys(iterator_to_array(new RecursiveDirectoryIterator(__DIR__)));
$ndots = array_keys(iterator_to_array(new RecursiveDirectoryIterator(__DIR__, FilesystemIterator::SKIP_DOTS)));

var_dump(in_array(__DIR__ . DIRECTORY_SEPARATOR . '.', $dots));
var_dump(in_array(__DIR__ . DIRECTORY_SEPARATOR . '..', $dots));

var_dump(in_array(__DIR__ . DIRECTORY_SEPARATOR . '.', $ndots));
var_dump(in_array(__DIR__ . DIRECTORY_SEPARATOR . '..', $ndots));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
