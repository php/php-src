--TEST--
Test that float to string and string to float casts are consistent
--SKIPIF--
<?php
if (!setlocale(
    LC_ALL,
    "german", "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8",
    "french", "fr", "fr_FR", "fr_FR.ISO8859-1", "fr_FR.ISO_8859-1", "fr_FR.UTF-8",
)) {
    die("skip locale needed for this test is not supported on this platform");
}
?>
--FILE--
<?php

setlocale(
    LC_ALL,
    "german", "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8",
    "french", "fr", "fr_FR", "fr_FR.ISO8859-1", "fr_FR.ISO_8859-1", "fr_FR.UTF-8",
);

$float = 1/3;
$string = (string) $float;
$float = (float) $string;

printf("%.2f", $float);

?>
--EXPECT--
0,33
