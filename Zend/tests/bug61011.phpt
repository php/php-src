--TEST--
Bug #61011 (Crash when an exception is thrown by __autoload accessing a static property)
--XFAIL--
#61011 have not been fixed yet
--FILE--
<?php
function __autoload($class) {
    throw new Exception($class);
}

try { 
NoExistsClass::$property;
} catch (Exception $e) {
}
echo 'okey';
--EXPECT--
okey
