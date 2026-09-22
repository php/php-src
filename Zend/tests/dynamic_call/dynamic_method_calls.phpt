--TEST--
Testing dynamic calls
--FILE--
<?php

class foo {
    static public function a() {
        print "ok\n";
    }
}

$a = 'a';
$b = 'a';

$class = 'foo';

foo::a();
foo::$a();
foo::$$b();

$class::a();
$class::$a();
$class::$$b();

?>
--EXPECT--
ok
ok
ok
ok
ok
ok
