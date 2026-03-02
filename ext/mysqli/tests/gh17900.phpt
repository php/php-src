--TEST--
GH-17900 (Assertion failure ext/mysqli/mysqli_prop.c)
--EXTENSIONS--
mysqli
--FILE--
<?php
mysqli_report(MYSQLI_REPORT_OFF);
$mysqli = new mysqli();
try {
    $mysqli->__construct('doesnotexist');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot call constructor twice
