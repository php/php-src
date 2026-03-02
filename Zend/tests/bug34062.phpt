--TEST--
Bug #34062 (Crash in catch block when many arguments are used)
--FILE--
<?php
function f1() { throw new Exception; }
function f2() { echo "here\n"; }

try {
      // Currently it's the minimum required number of zeros
      // If you remove one, it won't crash
    max(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, f1());
} catch (Exception $e) {
    echo "(((\n";
    f2(0, 0, 0); // Won't crash if less than 3 zeros here
    echo ")))\n";
}
?>
--EXPECT--
(((
here
)))
