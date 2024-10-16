--TEST--
032: Namespace support for user functions (php name)
--FILE--
<?php
class Test {
    static function foo() {
        echo __CLASS__,"::",__FUNCTION__,"\n";
    }
}

function foo() {
    echo __FUNCTION__,"\n";
}

call_user_func(__NAMESPACE__."\\foo");
call_user_func(__NAMESPACE__."\\test::foo");
?>
--EXPECT--
foo
Test::foo
