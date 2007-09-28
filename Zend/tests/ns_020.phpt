--TEST--
020: Accesing internal namespace function
--FILE--
<?php
namespace X;
import X as Y;
function foo() {
	echo __FUNCTION__,"\n";
}
foo();
X::foo();
Y::foo();
::X::foo();
--EXPECT--
X::foo
X::foo
X::foo
X::foo
