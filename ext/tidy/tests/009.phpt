--TEST--
tidy_doc object overloading
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
    
    $a = tidy_parse_string("<HTML></HTML>");
	echo $a;
 
?>
--EXPECT--
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>