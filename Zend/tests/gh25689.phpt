--TEST--
GH-25689 (Use-after-free in convert_to_*() when a destructor converts the same reference again)
--CREDITS--
xuziqiang98
--FILE--
<?php
class C {
    public function __destruct() {
        settype($GLOBALS['x'], $GLOBALS['type']);
    }
    public function __toString(): string {
        return 'C';
    }
}

foreach (['array', 'null', 'bool', 'string', 'int', 'float'] as $type) {
    $x = new C;
    settype($x, $type);
    var_dump($x);

    $x = [new C];
    settype($x, $type);
    var_dump($x);
}
echo 'Done', PHP_EOL;
?>
--EXPECTF--
array(0) {
}
array(1) {
  [0]=>
  object(C)#%d (0) {
  }
}
NULL
NULL
bool(true)
bool(true)
string(1) "C"

Warning: Array to string conversion in %s on line %d
string(5) "Array"

Warning: Object of class C could not be converted to int in %s on line %d
int(1)
int(1)

Warning: Object of class C could not be converted to float in %s on line %d
float(1)
float(1)
Done
