--TEST--
Test that exposing doc comments are supported by internal symbols
--EXTENSIONS--
zend_test
reflection
--FILE--
<?php

$rc = new ReflectionClass(ZendTestPropertyAttribute::class);
var_dump($rc->getDocComment());

$rc = new ReflectionClassConstant(_ZendTestInterface::class, "DUMMY");
var_dump($rc->getDocComment());

$rf = new ReflectionFunction("zend_test_nullable_array_return");
var_dump($rf->getDocComment());

$rp = new ReflectionProperty(new ZendTestPropertyAttribute(""), "parameter");
var_dump($rp->getDocComment());

?>
--EXPECT--
string(82) "/**
     * "Lorem ipsum"
     * @see https://www.php.net
     * @since 8.1
     */"
string(98) "/**
         * "Lorem ipsum"
         * @see https://www.php.net
         * @since 8.2
         */"
string(82) "/**
     * "Lorem ipsum"
     * @see https://www.php.net
     * @since 8.3
     */"
string(98) "/**
         * "Lorem ipsum"
         * @see https://www.php.net
         * @since 8.4
         */"
