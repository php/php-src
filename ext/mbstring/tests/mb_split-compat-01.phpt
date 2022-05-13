--TEST--
mb_split() compat test 1
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_split') or die("skip mb_split() is not available in this build");
?>
--FILE--
<?php
/* (counterpart: ext/standard/tests/reg/009.phpt) */
    $a=mb_split("[[:space:]]","this is	a
test");
    echo count($a) . "\n";
    for ($i = 0; $i < count($a); $i++) {
          echo $a[$i] . "\n";
        }
?>
--EXPECT--
4
this
is
a
test
