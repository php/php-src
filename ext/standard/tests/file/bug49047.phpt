--TEST--
Test fopen() function : variation: interesting paths, no use include path
--FILE--
<?php
// fopen with interesting windows paths.
$testdir = __DIR__ . '/bug47177.tmpdir';
mkdir($testdir);
$t = time() - 3600;
touch($testdir, $t);
clearstatcache();
$t2 = filemtime($testdir);
if ($t2 != $t) echo "failed (got $t2, expecting $t)\n";
rmdir($testdir);
echo "Ok.";
?>
--EXPECTF--
Ok.
