--TEST--
Asymmetric visibility on static props
--FILE--
<?php

class C {
    public private(set) static int $prop;
    public private(set) static array $prop2;
    public private(set) static stdClass $prop3;
    public private(set) static object $unset;

    public static function reset() {
        self::$prop = 1;
        self::$prop2 = [];
        self::$prop3 = new stdClass();
    }

    public static function setProp($prop) {
        self::$prop = $prop;
    }

    public static function addProp2($prop2) {
        self::$prop2[] = $prop2;
    }
}

function test() {
    C::reset();

    try {
        C::$prop = 2;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    C::setProp(3);
    var_dump(C::$prop);

    try {
        ++C::$prop;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    try {
        C::$prop++;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    try {
        C::$prop += str_repeat('a', 10);
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    try {
        $ref = &C::$prop;
        $ref++;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    try {
        $ref = 4;
        C::$prop = &$ref;
        $ref++;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop);

    try {
        C::$prop2[] = 'foo';
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    var_dump(C::$prop2);

    C::addProp2('bar');
    var_dump(C::$prop2);

    C::$prop3->foo = 'foo';
    var_dump(C::$prop3);

    unset(C::$unset->foo);
}

test();
echo "\nRepeat:\n";
test();

?>
--EXPECTF--
Error: Cannot modify private(set) property C::$prop from global scope
int(1)
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop2 from global scope
array(0) {
}
array(1) {
  [0]=>
  string(3) "bar"
}
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "foo"
}

Repeat:
Error: Cannot modify private(set) property C::$prop from global scope
int(1)
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop from global scope
int(3)
Error: Cannot indirectly modify private(set) property C::$prop2 from global scope
array(0) {
}
array(1) {
  [0]=>
  string(3) "bar"
}
object(stdClass)#%d (1) {
  ["foo"]=>
  string(3) "foo"
}
