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
--EXPECTF--
Deprecated: Function mb_split() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
4
this
is
a
test
