--TEST--
__COMPILER_HALT_OFFSET__ is a "magic" constant, which should work if referenced directly, even in a namespace
--FILE--
<?php

namespace {
	echo __COMPILER_HALT_OFFSET__, "\n";
	echo \__COMPILER_HALT_OFFSET__, "\n";
}

namespace Foo {
	echo __COMPILER_HALT_OFFSET__, "\n";
	echo \__COMPILER_HALT_OFFSET__, "\n";
	echo namespace\__COMPILER_HALT_OFFSET__, "\n";

}

__halt_compiler();

?>
--EXPECTF--
%d
%d
%d
%d

Fatal error: Uncaught Error: Undefined constant 'Foo\__COMPILER_HALT_OFFSET__' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
