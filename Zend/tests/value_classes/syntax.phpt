--TEST--
Value is a contextual, case insensitive class modifier
--FILE--
<?php
value /* comment */ class First {}
VaLuE // comment
class Second {}
value # comment
readonly final class Third {}
final readonly value class Fourth {}
readonly value final class Fifth {}
value/**/class Sixth {}

class value {
    public const value = 'constant';
    public const ?value other = null;

    public function value(): string { return 'method'; }
}
class TypedConstant { public const ?value value = null; }
function value(value $value): value { return $value; }
const value = 'global';

foreach ([First::class, Second::class, Third::class, Fourth::class, Fifth::class, Sixth::class] as $class) {
    $reflection = new ReflectionClass($class);
    var_dump($reflection->isFinal() && $reflection->isReadOnly());
}
var_dump(value(new value())->value(), value::value, value);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(6) "method"
string(8) "constant"
string(6) "global"
