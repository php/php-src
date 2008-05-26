--TEST--
Bug #27798 (private / protected variables not exposed by get_object_vars() inside class)
--FILE--
<?php

class Base
{
	public    $Foo = 1;
	protected $Bar = 2;
	private   $Baz = 3;
	
	function __construct()
	{
		echo __METHOD__ . "\n";
		var_dump(get_object_vars($this));
	}
}

class Child extends Base
{
	private   $Baz = 4;

	function __construct()
	{
		parent::__construct();
		echo __METHOD__ . "\n";
		var_dump(get_object_vars($this));
	}
}

var_dump(get_object_vars(new Base));
var_dump(get_object_vars(new Child));

?>
===DONE===
--EXPECT--
Base::__construct
array(3) {
  [u"Foo"]=>
  int(1)
  [u"Bar"]=>
  int(2)
  [u"Baz"]=>
  int(3)
}
array(1) {
  [u"Foo"]=>
  int(1)
}
Base::__construct
array(3) {
  [u"Foo"]=>
  int(1)
  [u"Bar"]=>
  int(2)
  [u"Baz"]=>
  int(3)
}
Child::__construct
array(3) {
  [u"Baz"]=>
  int(4)
  [u"Foo"]=>
  int(1)
  [u"Bar"]=>
  int(2)
}
array(1) {
  [u"Foo"]=>
  int(1)
}
===DONE===
