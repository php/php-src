--TEST--
The Tidy Output Buffer Filter
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php ob_start("ob_tidyhandler"); ?>
<B>testing</I>
--EXPECT--
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<head>
<title></title>
</head>
<body>
<b>testing</b>
</body>
</html>