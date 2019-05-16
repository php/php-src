--TEST--
The Tidy Output Buffer Filter
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php ob_start("ob_tidyhandler"); ?>
<B>testing</I>
--EXPECTF--
<!DOCTYPE html%S>
<html>
<head>
<title></title>
</head>
<body>
<b>testing</b>
</body>
</html>
