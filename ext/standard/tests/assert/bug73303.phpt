--TEST--
Bug #73303: Scope not inherited by eval in assert()
--FILE--
<?php

class Test {
	public $prop;

	public function main(){
		assert('self::checkCacheKey(get_object_vars($this))');
		echo 'Success';
	}
	private static function checkCacheKey($obj_properties){
		return count($obj_properties) == 1;
	}
}

$obj = new Test();
$obj->main();

?>
--EXPECTF--
Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
Success
