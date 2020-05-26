--TEST--
Bug #73927 (phpdbg fails with windows error prompt at "watch array")
--PHPDBG--
b 19
r
c
w $value
w $lower[]
q
--EXPECT--
[Successful compilation of C:\php-sdk\phpdev\vc15\x64\php-src-pr5599\sapi\phpdbg\tests\bug73927.php]
prompt> [Breakpoint #0 added at C:\php-sdk\phpdev\vc15\x64\php-src-pr5599\sapi\phpdbg\tests\bug73927.php:19]
prompt> [Breakpoint #0 at C:\php-sdk\phpdev\vc15\x64\php-src-pr5599\sapi\phpdbg\tests\bug73927.php:19, hits: 1]
>00019:     if ($value < 100) {
 00020:         $lower[] = $value;
 00021:     } else {
prompt> [Breakpoint #0 at C:\php-sdk\phpdev\vc15\x64\php-src-pr5599\sapi\phpdbg\tests\bug73927.php:19, hits: 2]
>00019:     if ($value < 100) {
 00020:         $lower[] = $value;
 00021:     } else {
prompt> [Added watchpoint #0 for $value]
prompt> [Added watchpoint #1 for $lower[0]]
prompt> [$lower[0] has been removed, removing watchpoint]
[$lower[0] has been removed, removing watchpoint]
[$value has been removed, removing watchpoint]
--FILE--
<?php

// Generate some mock data
$example = [1, 23, 23423, 256436, 3463, 4363, 457];
foreach (range(1, 1000) as $val) {
    $example[] = mt_rand(1, 10000);
}

// Stuff to debug
function doCoolStuff($value)
{
    $value++;

    return mt_rand(1, 1000);
}

$lower = [];
foreach ($example as $key => $value) {
    if ($value < 100) {
        $lower[] = $value;
    } else {
        doCoolStuff($value);
    }
}

?>
