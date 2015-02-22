--TEST--
Test if exceeding arg count check works on methods
--FILE--
<?php

class Foo {
    public function method($arg) {
        echo __METHOD__, PHP_EOL;
    }

    public function method_func_get_args_sensitive($arg) {
        func_get_args();
        echo __METHOD__, PHP_EOL;
    }

    public static function static_method($arg) {
        echo __METHOD__, PHP_EOL;
    }

    public static function static_method_func_get_args_sensitive($arg) {
        func_get_args();
        echo __METHOD__, PHP_EOL;
    }
}

(new Foo)->method(1);
(new Foo)->method();
(new Foo)->method(1, 2);
call_user_func_array([new Foo, "method"], [1, 2]);

echo PHP_EOL, "OK1", PHP_EOL;

Foo::static_method(1);
Foo::static_method();
Foo::static_method(1, 2);
call_user_func_array(["Foo", "static_method"], [1, 2]);

echo PHP_EOL, "OK2", PHP_EOL;

(new Foo)->method_func_get_args_sensitive(1);
(new Foo)->method_func_get_args_sensitive();
(new Foo)->method_func_get_args_sensitive(1, 2);
call_user_func_array([new Foo, "method_func_get_args_sensitive"], [1, 2]);

echo PHP_EOL, "OK3", PHP_EOL;

Foo::static_method_func_get_args_sensitive(1);
Foo::static_method_func_get_args_sensitive();
Foo::static_method_func_get_args_sensitive(1, 2);
call_user_func_array(["Foo", "static_method_func_get_args_sensitive"], [1, 2]);

echo PHP_EOL, "Done", PHP_EOL;

--EXPECTF--
Foo::method

Warning: Missing argument 1 for Foo::method(), called in %s on line 24 and defined in %s on line 4
Foo::method

Warning: Foo::method() expects at most 1 parameter, 2 given, defined in %s on line 4 and called in %s on line 25
Foo::method

Warning: Foo::method() expects at most 1 parameter, 2 given, defined in %s on line 4 and called in %s on line 26
Foo::method

OK1
Foo::static_method

Warning: Missing argument 1 for Foo::static_method(), called in %s on line 31 and defined in %s on line 13
Foo::static_method

Warning: Foo::static_method() expects at most 1 parameter, 2 given, defined in %s on line 13 and called in %s on line 32
Foo::static_method

Warning: Foo::static_method() expects at most 1 parameter, 2 given, defined in %s on line 13 and called in %s on line 33
Foo::static_method

OK2
Foo::method_func_get_args_sensitive

Warning: Missing argument 1 for Foo::method_func_get_args_sensitive(), called in %s on line 38 and defined in %s on line 8
Foo::method_func_get_args_sensitive
Foo::method_func_get_args_sensitive
Foo::method_func_get_args_sensitive

OK3
Foo::static_method_func_get_args_sensitive

Warning: Missing argument 1 for Foo::static_method_func_get_args_sensitive(), called in %s on line 45 and defined in %s on line 17
Foo::static_method_func_get_args_sensitive
Foo::static_method_func_get_args_sensitive
Foo::static_method_func_get_args_sensitive

Done
