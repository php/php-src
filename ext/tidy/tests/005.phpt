--TEST--
tidy_parse_file()
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 

    $tidy = tidy_create();
    
    tidy_parse_file($tidy, "ext/tidy/tests/005.html");
    
    echo tidy_get_output($tidy);

?>
--EXPECT--
<html>
<head>
<title></title>
</head>
<body>
</body>
</html>