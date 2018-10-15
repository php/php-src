--TEST--
tidy_parse_file()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
	$a = tidy_parse_file(dirname(__FILE__)."/005私はガラスを食べられます.html");
	echo tidy_get_output($a);

?>
--EXPECT--
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>
