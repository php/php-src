--TEST--
Bug #63882_2 (arsort crash on recursion)
--FILE--
<?php
$token = array();
$conditions = array();
for ($i = 0; $i <= 2; $i++) {
    $tokens = $conditions;
    $a[0] =& $a;
    $a = unserialize(serialize($GLOBALS));
    $a[0] =& $a;
    $a = unserialize(serialize($GLOBALS));
    $a[0] =& $a;
    foreach($a as $v) {
        if ($v == 1) {
            arsort($a);
        }
    }
}
?>
DONE
--EXPECT--
DONE
