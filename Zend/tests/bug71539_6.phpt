--TEST--
Bug #71539.5 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php
$name = 'a';
for ($i = 0; $i < 100000; $i++) {
    if ($name != 'i') {
        $$name =& $GLOBALS;
    }
    $name++;
}
?>
OK
--EXPECT--
OK
