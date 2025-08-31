--TEST--
sprintf() %h and %H specifiers
--SKIPIF--
<?php
if (setlocale(LC_ALL, 'invalid') === 'invalid') { die('skip setlocale() is broken /w musl'); }
if (!setlocale(LC_ALL, "de_DE.utf8")) die("skip de_DE.utf8 locale not available");
?>
--FILE--
<?php

setlocale(LC_ALL, "de_DE.utf8");
$f = 1.25;
printf("%g %G %h %H\n", $f, $f, $f, $f);
$f = 0.00000125;
printf("%g %G %h %H\n", $f, $f, $f, $f);

?>
--EXPECT--
1,25 1,25 1.25 1.25
1,25e-6 1,25E-6 1.25e-6 1.25E-6
