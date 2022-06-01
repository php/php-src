--TEST--
Calling a builtin function with 'static' return type from internal code
--FILE--
<?php

enum IntIntStaticInt : int {
	case Life = 42;
}

enum IntIntStaticString : string {
	case ThanksFor = "all the fish";
}

var_dump(zend_call_method("IntIntStaticInt", "from", 42));
var_dump(zend_call_method("IntIntStaticInt", "tryFrom", 42));
var_dump(zend_call_method("IntIntStaticString", "from", "all the fish"));
var_dump(zend_call_method("IntIntStaticString", "tryFrom", "all the fish"));

class StillReturnsStatic extends _ZendTestClass {}

var_dump(get_class(zend_call_method("_ZendTestClass", "returnsStatic")));
var_dump(get_class(zend_call_method("StillReturnsStatic", "returnsStatic")));

--EXPECT--
enum(IntIntStaticInt::Life)
enum(IntIntStaticInt::Life)
enum(IntIntStaticString::ThanksFor)
enum(IntIntStaticString::ThanksFor)
string(14) "_ZendTestClass"
string(18) "StillReturnsStatic"
