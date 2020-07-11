--TEST--
jump 09: goto in declare statement
--FILE--
<?php
a: print "ok!\n";
goto c;
declare (ticks=1) {
    b:
        print "ok!\n";
        exit;
}
c:
    print "ok!\n";
    goto b;
?>
--EXPECT--
ok!
ok!
ok!
