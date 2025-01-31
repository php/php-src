--TEST--
Early break
--FILE--
<?php
function test($a,$b) {
    $b ?? return 0;
    $a ?: return 1;
    $c = $a <= 0 ? return "negative" : $a*2;

    return 2;
}
echo test(1, null);
echo test(0, 1);
echo test(-1, 1);
echo test(3, 1);
?>
--EXPECT--
01negative2
