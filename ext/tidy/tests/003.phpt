--TEST--
tidy_clean_repair()
--SKIPIF--
<?php if (!extension_loaded("Tidy")) print "skip"; ?>
--POST--
--GET--
--INI--
--FILE--
<?php 

    $tidy = tidy_create();
    
    tidy_parse_string($tidy, "<HTML></HTML>");
    tidy_clean_repair($tidy);

    echo tidy_get_output($tidy);

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
