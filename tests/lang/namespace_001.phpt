--TEST--
ZE2 namespaces
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

namespace Foo {
	var $bar;
    
	function SomeFunction() {
		echo "I'm Foo::SomeFunction()! Foo::\$bar is set to: " . Foo::$bar;
	}
    
	class SomeClass {
		function __construct() {
			echo "I'm Foo::SomeClass::__construct()!\n";
			echo Foo::$bar;
		}
	}
}

Foo::$bar = "I'm Foo::\$bar!\n";
Foo::SomeFunction();
$someClass = new Foo::SomeClass;

?>
--EXPECT--
I'm Foo::SomeFunction()! Foo::$bar is set to: I'm Foo::$bar!
I'm Foo::SomeClass::__construct()!
I'm Foo::$bar!
