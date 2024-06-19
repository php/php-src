--TEST--
phpdbg_watch null pointer access
--CREDITS--
Yuancheng Jiang
--SKIPIF--
<?php
if (getenv('SKIP_ASAN')) {
    die("skip intentionally causes segfaults");
}
?>
--FILE--
<?php
echo "*** Testing array_multisort() : Testing with anonymous arguments ***\n";
var_dump(array_multisort(array(1,3,2,4)));
$xconnect=$GLOBALS[array_rand($GLOBALS)];
echo "Done\n";
$a = [];
$a[0] = 1;
$a[0] = 2;
$a = [0 => 3, 1 => 4];
?>
--PHPDBG--
b 6
r
w a $a
c
q
--EXPECTF--
[Successful compilation of %s]
prompt> [Breakpoint #0 added at %s:%d]
prompt> *** Testing array_multisort() : Testing with anonymous arguments ***
bool(true)
Done
[Breakpoint #0 at %s:%d, hits: 1]
>00006: $a = [];
 00007: $a[0] = 1;
 00008: $a[0] = 2;
prompt> prompt> [Script ended normally]
prompt>
