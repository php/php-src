--TEST--
jump 05: goto from loop (forward)
--FILE--
<?php
$ar = array("1","2","3");
foreach ($ar as $val) {
    switch ($val) {
        case "1":
            echo "1: ok\n";
            break;
        case "2":
            echo "2: ok\n";
            goto L1;
        case "3":
            echo "bug\n";
            break;
    }
}
echo "bug\n";
L1:
echo "3: ok\n";
?>
--EXPECT--
1: ok
2: ok
3: ok
