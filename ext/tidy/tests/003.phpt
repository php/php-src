--TEST--
tidy_clean_repair()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
	if (class_exists("tidy_doc")) {
		$a = tidy_parse_string("<HTML></HTML>");
		tidy_clean_repair($a);
		echo tidy_get_output($a);
	} else {
		tidy_parse_string("<HTML></HTML>");
		tidy_clean_repair();
		echo tidy_get_output();
	}
?>
--EXPECT--
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 3.2//EN">
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>
