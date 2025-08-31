--TEST--
tidy_doc object overloading
--EXTENSIONS--
tidy
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
