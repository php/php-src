--TEST--
Exception occuring while passing arguments to a handler
--FILE--
<?php

use extension string StringHandler;

function do_throw() {
    throw new Exception;
}

class StringHandler {
    public static function test($self, $a) {
        echo "Called test()\n";
    }
}

$str = "abc";
$str2 = "def";

try {
    $str->test(do_throw());
} catch (Exception $e) {
    echo $e, "\n";
}

try {
    $str->test($str2, do_throw());
} catch (Exception $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
Exception in %s:%d
Stack trace:
#0 %s(%d): do_throw()
#1 {main}
Exception in %s:%d
Stack trace:
#0 %s(%d): do_throw()
#1 {main}
