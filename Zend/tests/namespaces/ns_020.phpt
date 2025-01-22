--TEST--
020: Accessing internal namespace function
--FILE--
<?php
namespace X;
use X as Y;
function foo() {
    echo __FUNCTION__,"\n";
}
foo();
\X\foo();
Y\foo();
\X\foo();
?>
--EXPECT--
X\foo
X\foo
X\foo
X\foo
