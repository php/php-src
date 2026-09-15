--TEST--
GH-17773 (Initializing array element with reference returned by function)
--FILE--
<?php
function &get_reference() {
    static $value = 0;
    return $value;
}

$array = [&get_reference()];
$array[0] = 42;
var_dump(get_reference());

class Test {
    public int $value = 0;
    public static int $staticValue = 0;

    public function &getReference() {
        return $this->value;
    }

    public static function &getStaticReference() {
        return self::$staticValue;
    }
}

$test = new Test();
$array = [&$test->getReference(), &Test::getStaticReference(), [&get_reference()]];
$unpacked = [...$array];
$unpacked[0] = 43;
$unpacked[1] = 44;
$unpacked[2][0] = 45;
var_dump($test->value, Test::$staticValue, get_reference());

function get_value() {
    return 42;
}

$array = [&get_value()];
var_dump($array);
?>
--EXPECTF--
int(42)
int(43)
int(44)
int(45)

Notice: Only variables should be assigned by reference in %s on line %d
array(1) {
  [0]=>
  int(42)
}
