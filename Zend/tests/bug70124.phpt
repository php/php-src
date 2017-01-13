--TEST--
Bug #70124 (null ptr deref / seg fault in ZEND_HANDLE_EXCEPTION_SPEC_HANDLER)
--FILE--
<?php 

try  {
	echo base_convert([array_search(chr(48),chr(48),chr(48),chr(48),chr(48),$f("test"))],chr(48));
} catch (Error $e) {
	var_dump($e->getMessage());
}

class A {
}

try  {
	echo base_convert([array_search(chr(48),chr(48),chr(48),chr(48),chr(48),a::y("test"))],chr(48));
} catch (Error $e) {
	var_dump($e->getMessage());
}

$a = new A;

try  {
	echo base_convert([array_search(chr(48),chr(48),chr(48),chr(48),chr(48),$a->y("test"))],chr(48));
} catch (Error $e) {
	var_dump($e->getMessage());
}

try  {
	echo base_convert([array_search(chr(48),chr(48),chr(48),chr(48),chr(48),\bar\y("test"))],chr(48));
} catch (Error $e) {
	var_dump($e->getMessage());
}

try  {
	echo base_convert([array_search(chr(48),chr(48),chr(48),chr(48),chr(48),y("test"))],chr(48));
} catch (Error $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
Notice: Undefined variable: f in %sbug70124.php on line %d
string(30) "Function name must be a string"
string(31) "Call to undefined method A::y()"
string(31) "Call to undefined method A::y()"
string(34) "Call to undefined function bar\y()"
string(30) "Call to undefined function y()"
