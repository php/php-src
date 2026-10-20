--TEST--
GH-20678 (resource created by GlobalIterator crashes when it is called with fclose())
--CREDITS--
chongwick
--FILE--
<?php
$iter = new GlobIterator(__DIR__ . '/*.abcdefghij');
$resources = get_resources();
$resource = end($resources);
fclose($resource);
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
