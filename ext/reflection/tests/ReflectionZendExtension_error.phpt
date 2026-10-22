--TEST--
Test ReflectionZendExtension class errors
--CREDITS--
Gabriel Caruso (carusogabriel34@gmail.com)
--EXTENSIONS--
opcache
--FILE--
<?php
try {
    new ReflectionZendExtension('zend_opcache');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionException: Zend Extension "zend_opcache" does not exist
