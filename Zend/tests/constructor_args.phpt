--TEST--
Different numbers of arguments in __construct()
--FILE--
<?php
interface foobar {
    function __construct();
}
abstract class bar implements foobar {
    public function __construct($x = 1) {
    }
}
final class foo extends bar implements foobar {
    public function __construct($x = 1, $y = 2) {
    }
}
new foo;
print "ok!";
?>
--EXPECT--
ok!
