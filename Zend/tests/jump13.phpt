--TEST--
jump 10: goto with try blocks
--FILE--
<?php
goto a;
e: return;
try {
    a: print 1;
    goto b;
    try {
        b: print 2;
        goto c;
    }
    catch(Exception $e) {
        c: print 3;
        goto d;
    }
}
catch(Exception $e) {
    d: print 4;
    goto e;
}
?>
--EXPECT--
1234
