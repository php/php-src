--TEST--
Trait methods: substituted return type is enforced when the body returns a hardcoded wrong-type value
--FILE--
<?php
trait Maker<T> {
    public function makeStr(): T { return "hello"; }
    public function makeArr(): T { return [1, 2]; }
}

class IntUser { use Maker<int>; }
class StringUser { use Maker<string>; }

try {
    (new IntUser())->makeStr();
} catch (TypeError $e) {
    echo "int-str: ", $e->getMessage(), "\n";
}

try {
    (new IntUser())->makeArr();
} catch (TypeError $e) {
    echo "int-arr: ", $e->getMessage(), "\n";
}

try {
    (new StringUser())->makeArr();
} catch (TypeError $e) {
    echo "str-arr: ", $e->getMessage(), "\n";
}

var_dump((new StringUser())->makeStr());
?>
--EXPECT--
int-str: IntUser::makeStr(): Return value must be of type int, string returned
int-arr: IntUser::makeArr(): Return value must be of type int, array returned
str-arr: StringUser::makeArr(): Return value must be of type string, array returned
string(5) "hello"
