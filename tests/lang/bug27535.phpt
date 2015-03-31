--TEST--
Bug #27535 (Objects pointing to each other cause Apache to crash)
--FILE--
<?php
                                                                                                                                
class Class1
{
	public $_Class2_obj;
}

class Class2
{
	public $storage = '';

	function __construct()
	{
		$this->storage = new Class1();

		$this->storage->_Class2_obj = $this;
	}
}

$foo = new Class2();

?>
Alive!
--EXPECT--
Alive!
