--TEST--
Inherited methods: variadic T with a leading non-generic parameter — both positions enforce their substituted types
--FILE--
<?php
class Box<T> {
    public function take(int $count, T ...$rest): void {
        echo "count=$count rest=";
        var_dump($rest);
    }
}

class StringBox extends Box<string> {}

$b = new StringBox();
$b->take(2, "a", "b");

try {
    $b->take("non-numeric", "a");
} catch (TypeError $e) {
    echo "lead: ", $e->getMessage(), "\n";
}

try {
    $b->take(1, "ok", [1, 2]);
} catch (TypeError $e) {
    echo "var: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
count=2 rest=array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
lead: Box::take(): Argument #1 ($count) must be of type int, string given, called in %s on line %d
var: Box::take(): Argument #3 must be of type string, array given, called in %s on line %d
