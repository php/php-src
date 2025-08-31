--TEST--
Leak 001: Incorrect 'if ();' optimization
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_cache_only=0
--EXTENSIONS--
opcache
--FILE--
<?php
if (false);

if (true);

if (2 + 3);

$x = 2;
$y = 3;
if ($x + $y);

if ($x);

$a = [[$x]];
if ($a[0]);

if (new stdClass());

$x = 2;
$a = [1,$x];
if ((object)$a);
?>
OK
--EXPECT--
OK
