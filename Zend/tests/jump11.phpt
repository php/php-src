--TEST--
jump 08: goto inside switch in constructor
--FILE--
<?php
class foobar {
    public function __construct() {
        switch (1) {            
            default:
                goto b;
                a:
                    print "ok!\n";
                    break;               
                b:
                    print "ok!\n";
                    goto a;
        }        
        print "ok!\n";
    }
}

new foobar;
?>
--EXPECT--
ok!
ok!
ok!
