--TEST--
Testing 'new static;' calling parent method
--FILE--
<?php 

class bar {
	public function show() {
		var_dump(new static);
	}
}
 
class foo extends bar {
	public function test() {
		parent::show();
	}
}
 
$foo = new foo;
$foo->test();
$foo::test();

call_user_func(array($foo, 'test'));
call_user_func(array('foo', 'test'));

?>
--EXPECTF--
object(foo)#%d (0) {
}

Deprecated: Non-static method foo::test() should not be called statically in %s on line %d

Deprecated: Non-static method bar::show() should not be called statically in %s on line %d
object(foo)#%d (0) {
}
object(foo)#%d (0) {
}

Deprecated: %son-static method foo::test() should not be called statically in %s on line %d

Deprecated: Non-static method bar::show() should not be called statically in %s on line %d
object(foo)#%d (0) {
}


