--TEST--
021: Name search priority (first look into namespace)
--FILE--
<?php
namespace test;

class Test {
    static function foo() {
        echo __CLASS__,"::",__FUNCTION__,"\n";
    }
}

function foo() {
    echo __FUNCTION__,"\n";
}

foo();
\test\foo();
\test\test::foo();
?>
--EXPECT--
test\foo
test\foo
test\Test::foo
