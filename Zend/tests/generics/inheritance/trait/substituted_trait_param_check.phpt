--TEST--
Trait methods: substituted parameter type is enforced at runtime on the using class
--FILE--
<?php
trait Holder<T> {
    public function take(T $v): void {
        var_dump($v);
    }
}

class IntHolder { use Holder<int>; }
class StringHolder { use Holder<string>; }

(new IntHolder())->take(42);
(new StringHolder())->take("hi");

try {
    (new IntHolder())->take("hello");
} catch (TypeError $e) {
    echo "int: ", $e->getMessage(), "\n";
}

try {
    (new StringHolder())->take([1]);
} catch (TypeError $e) {
    echo "str: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
int(42)
string(2) "hi"
int: IntHolder::take(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
str: StringHolder::take(): Argument #1 ($v) must be of type string, array given, called in %s on line %d
