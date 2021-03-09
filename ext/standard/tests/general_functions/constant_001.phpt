--TEST--
Test constant() function: test access to class constant
--FILE--
<?php
namespace Foo;

class Bar {}

var_dump(constant('Foo\Bar::class'));
?>
--EXPECT--
string(7) "Foo\Bar"
