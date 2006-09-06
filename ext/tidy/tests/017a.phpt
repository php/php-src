--TEST--
The Tidy Output Buffer Filter
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php ob_start("ob_tidyhandler", 1234); ?>
<B>testing</I>
--EXPECTF--
Warning: ob_start(): Cannot use a chunk size for ob_tidyhandler in %s017a.php on line 1

Notice: ob_start(): failed to create buffer in %s017a.php on line 1
<B>testing</I>
