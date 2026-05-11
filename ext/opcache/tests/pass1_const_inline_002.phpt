--TEST--
Global and namespace const declarations are inlined by the optimizer
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.opt_debug_level=0x20000
--FILE--
<?php

const GLOBAL_CONST = 42;

function use_global_const() {
    return GLOBAL_CONST;
}

function use_global_const_fqn() {
    return \GLOBAL_CONST;
}

class MyClass {
    const CLASS_CONST = 99;
    final const FINAL_CLASS_CONST = 77;

    public function use_class_const() {
        return self::CLASS_CONST;
    }

    public function use_static_class_const() {
        return static::CLASS_CONST;
    }

    public function use_static_final_class_const() {
        return static::FINAL_CLASS_CONST;
    }
}

function use_class_const_static() {
    return MyClass::CLASS_CONST;
}

?>
--EXPECTF--
$_main:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 DECLARE_CONST string("GLOBAL_CONST") int(42)
0001 RETURN int(1)

use_global_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

use_global_const_fqn:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(42)

use_class_const_static:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(99)

MyClass::use_class_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(99)

MyClass::use_static_class_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 T%d = FETCH_CLASS_CONSTANT (static) (exception) string("CLASS_CONST")
0001 RETURN %s

MyClass::use_static_final_class_const:
     ; (lines=%d, args=%d, vars=%d, tmps=%d)
     ; (after optimizer)
     ; %s
0000 RETURN int(77)
