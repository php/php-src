--TEST--
tidy_parse_string()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
    	$a = tidy_parse_string("<HTML></HTML>");
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