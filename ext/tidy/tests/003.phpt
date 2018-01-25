--TEST--
tidy_clean_repair()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

	$a = tidy_parse_string("<HTML></HTML>");
	tidy_clean_repair($a);
	echo tidy_get_output($a);

?>
--EXPECTF--
<!DOCTYPE html%S>
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>
