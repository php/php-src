--TEST--
Array callbacks work from within handlers
--FILE--
<?php

use extension string StringHandler;

class StringHandler {
    public static function test($self) {
        $callback = [__CLASS__, 'test2'];
        $callback();
        call_user_func($callback);
    }

    public static function test2() {
        echo "Called test2()\n";
    }
}

$str = "foo";
$str->test();

?>
--EXPECT--
Called test2()
Called test2()
