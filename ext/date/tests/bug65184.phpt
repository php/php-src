--TEST--
Test bug #65184 strftime() returns insufficient-length string under multibyte locales
--SKIPIF--
<?php
if (strtoupper(substr(PHP_OS, 0, 3)) != 'WIN') {
    die("skip Test is valid for Windows");
}
?>
--INI--
date.timezone = UTC
--FILE--
<?php
    setlocale(LC_ALL, 'Japanese_Japan.932');
    /* timestamp has to be some wednesday */
    $s = strftime('%A', 1372884126);

    for ($i = 0; $i < strlen($s); $i++) {
        printf("%x ", ord($s[$i]));
    }
    echo "\n";

    echo strlen(strftime('%A')), "\n";
?>
--EXPECT--
90 85 97 6a 93 fa 
6
