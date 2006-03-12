--TEST--
Reflection Bug #30856 (ReflectionClass::getStaticProperties segfaults)
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php
class bogus {
        const C = 'test';
        static $a = bogus::C;
}

$class = new ReflectionClass('bogus');

var_dump($class->getStaticProperties());
?>
===DONE===
--EXPECT--
array(1) {
  ["a"]=>
  string(4) "test"
}
===DONE===
