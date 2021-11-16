--TEST--
tidy_parse_file()
--EXTENSIONS--
tidy
--FILE--
<?php
    $a = tidy_parse_file(__DIR__."/005私はガラスを食べられます.html");
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
