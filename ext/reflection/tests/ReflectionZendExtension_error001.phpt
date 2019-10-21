--TEST--
ReflectionZendExtension: error with invalid extension name
--FILE--
<?php
try {
    new ReflectionZendExtension('foo-bar-baz');
} catch (ReflectionException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Zend Extension foo-bar-baz does not exist
