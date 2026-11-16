--TEST--
Implicit conversion of objects to string for typed properties is deprecated
--FILE--
<?php

class Str {
    public function __toString(): string {
        echo "__toString()\n";
        return 'str';
    }
}

class Test {
    public string $s = '';
    public int|string $u = 0;
    public static ?string $static = null;
}

$t = new Test;
$t->s = new Str;
var_dump($t->s);

$t->u = new Str;
var_dump($t->u);

Test::$static = new Str;
var_dump(Test::$static);

$ref = &$t->s;
$ref = new Str;
var_dump($t->s);
unset($ref);

try {
    $t->s = new stdClass;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

class Gadget {
    public string $s;

    public function __unserialize(array $data): void {
        $this->s = $data['s'];
    }
}

var_dump(unserialize('O:6:"Gadget":1:{s:1:"s";O:3:"Str":0:{}}'));

set_error_handler(function ($errno, $errstr) {
    throw new ErrorException($errstr, 0, $errno);
});

$t->s = 'unchanged';
try {
    $t->s = new Str;
} catch (ErrorException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($t->s);

?>
--EXPECTF--
Deprecated: Implicit conversion from Str to string for property Test::$s of type string is deprecated in %s on line %d
__toString()
string(3) "str"

Deprecated: Implicit conversion from Str to string for property Test::$u of type string|int is deprecated in %s on line %d
__toString()
string(3) "str"

Deprecated: Implicit conversion from Str to string for property Test::$static of type ?string is deprecated in %s on line %d
__toString()
string(3) "str"

Deprecated: Implicit conversion from Str to string for reference held by property Test::$s of type string is deprecated in %s on line %d
__toString()
string(3) "str"
Cannot assign stdClass to property Test::$s of type string

Deprecated: Implicit conversion from Str to string for property Gadget::$s of type string is deprecated in %s on line %d
__toString()
object(Gadget)#%d (1) {
  ["s"]=>
  string(3) "str"
}
Implicit conversion from Str to string for property Test::$s of type string is deprecated
string(9) "unchanged"
