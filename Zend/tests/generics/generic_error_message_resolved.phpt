--TEST--
Generic class: error messages show resolved type names
--FILE--
<?php
class Container<T> {
    public function __construct(T $value) {}
}

// Should show "int" not "T" in error message
try {
    $c = new Container<int>("hello");
} catch (TypeError $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
%s::__construct(): Argument #1 ($value) must be of type int, string given, called in %s on line %d
