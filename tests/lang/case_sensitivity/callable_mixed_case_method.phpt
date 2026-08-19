--TEST--
Callables referencing mixed-case method names resolve with exact case
--FILE--
<?php
class Foo {
    public function barBaz(): string { return 'instance'; }
    public static function statMethod(): string { return 'static'; }
}

var_dump(call_user_func([new Foo, 'barBaz']));
var_dump(call_user_func(['Foo', 'statMethod']));
var_dump(call_user_func('Foo::statMethod'));
var_dump(is_callable([new Foo, 'barbaz']));
var_dump(is_callable('Foo::STATMETHOD'));
?>
--EXPECT--
string(8) "instance"
string(6) "static"
string(6) "static"
bool(false)
bool(false)
