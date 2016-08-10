--TEST--
tidy.clean_output test
--SKIPIF--
<?php if (!extension_loaded('tidy')) die('skip'); ?>
--INI--
tidy.clean_output=1
--FILE--
<html>
<?php

echo '<p>xpto</p>';

?>
</html>
--EXPECTF--
<!DOCTYPE html%S>
<html>
<head>
<title></title>
</head>
<body>
<p>xpto</p>
</body>
</html>
