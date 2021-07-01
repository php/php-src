--TEST--
mysqli get_client_info
--EXTENSIONS--
mysqli
--FILE--
<?php
    $s = mysqli_get_client_info();
    echo gettype($s);
?>
--EXPECT--
string
