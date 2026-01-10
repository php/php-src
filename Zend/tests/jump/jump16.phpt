--TEST--
jump 16: goto into try/catch
--FILE--
<?php
goto a;
try {
    echo "1";
a:
    echo "2";
    throw new Exception();
} catch (Exception $e) {
    echo "3";
}
echo "4";
goto b;
try {
    echo "5";
    throw new Exception();
} catch (Exception $e) {
    echo "6";
b:
    echo "7";
}
echo "8\n";
?>
--EXPECT--
23478
