--TEST--
tidy_parse_string()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 
    tidy_parse_string("<HTML></HTML>");
    
    echo tidy_get_output();

?>
--EXPECT--
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>