--TEST--
Calling a builtin function with 'static' return type from internal code
--EXTENSIONS--
zend_test
--FILE--
<?php

enum IntIntStaticInt : int {
	case Life = 42;
}

enum IntIntStaticString : string {
	case ThanksFor = "all the fish";
}

var_dump(zend_call_method(IntIntStaticInt::class, "from", 42));
var_dump(zend_call_method(IntIntStaticInt::class, "tryFrom", 42));
var_dump(zend_call_method(IntIntStaticString::class, "from", "all the fish"));
var_dump(zend_call_method(IntIntStaticString::class, "tryFrom", "all the fish"));

class StillReturnsStatic extends _ZendTestClass {}

var_dump(get_class(zend_call_method(_ZendTestClass::class, "returnsStatic")));
var_dump(get_class(zend_call_method(StillReturnsStatic::class, "returnsStatic")));

--EXPECT--
enum(IntIntStaticInt::Life)
enum(IntIntStaticInt::Life)
enum(IntIntStaticString::ThanksFor)
enum(IntIntStaticString::ThanksFor)
string(14) "_ZendTestClass"
string(18) "StillReturnsStatic"
