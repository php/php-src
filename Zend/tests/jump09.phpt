--TEST--
jump 09: goto into switch (backward)
--FILE--
<?php
switch (0) {
    case 1:
        L1: echo "bug\n";
        break;
}
goto L1;
?>
--EXPECTF--
Fatal error: goto into a loop or switch statement is disallowed in %s on line %d
