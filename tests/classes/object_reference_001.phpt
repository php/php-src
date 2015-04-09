--TEST--
ZE2 object references
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Foo {
	public $name;
    
	function Foo() {
		$this->name = "I'm Foo!\n";
	}
}

$foo = new Foo;
echo $foo->name;
$bar = $foo;
$bar->name = "I'm Bar!\n";

// In ZE1, we would expect "I'm Foo!"
echo $foo->name;

?>
--EXPECT--
I'm Foo!
I'm Bar!
