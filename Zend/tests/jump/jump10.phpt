--TEST--
jump 10: goto into switch (forward)
--FILE--
<?php
goto L1;
switch (0) {
    case 1:
        L1: echo "bug\n";
        break;
}
?>
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump10.php on line 2
