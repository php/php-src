--TEST--
ZE2 A derived class does not know about privates of ancestors
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class Bar {
	public function pub() {
		$this->priv();
	}
	private function priv()	{
		echo "Bar::priv()\n";
	}
}
class Foo extends Bar {
	public function priv()	{ 
		echo "Foo::priv()\n";
	}
}

$obj = new Foo();
$obj->pub();
$obj->priv();

echo "Done\n";
?>
--EXPECTF--
Bar::priv()
Foo::priv()
Done
