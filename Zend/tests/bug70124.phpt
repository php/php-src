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
Warning: Undefined variable $f in %s on line %d
string(34) "Value of type null is not callable"
string(31) "Call to undefined method A::y()"
string(31) "Call to undefined method A::y()"
string(34) "Call to undefined function bar\y()"
string(30) "Call to undefined function y()"
