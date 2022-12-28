--TEST--
posix_ttyname(): manually validating int ZPP param
--EXTENSIONS--
posix
gmp
--FILE--
<?php

class classWithToString {
    public function __toString() {
        return "string";
    }
}

$types = [
    'null' => null,
    'false' => false,
    'true' => true,
    'int' => 1,
    'float no decimal' => 1.0,
    'float decimal' => 5.5,
    'string int' => "1",
    'string float no decimal' => "1.0",
    'string float decimal' => "5.5",
    'string' => "Hello",
    'array' => [],
    'class' => new stdClass(),
    'stringable class' => new classWithToString(),
    'int castable class' => gmp_init(1),
];

foreach ($types as $description => $type) {
    echo $description, ':';
    var_dump(posix_ttyname($type));
}
?>
--EXPECTF--
null:
Deprecated: posix_ttyname(): Passing null to parameter #1 ($file_descriptor) of type int is deprecated in %s on line %d
bool(false)
false:bool(false)
true:bool(false)
int:bool(false)
float no decimal:bool(false)
float decimal:
Deprecated: Implicit conversion from float 5.5 to int loses precision in %s on line %d
bool(false)
string int:bool(false)
string float no decimal:bool(false)
string float decimal:
Deprecated: Implicit conversion from float-string "5.5" to int loses precision in %s on line %d
bool(false)
string:
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, string given in %s on line %d
bool(false)
array:
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, array given in %s on line %d
bool(false)
class:
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, stdClass given in %s on line %d

Warning: Object of class stdClass could not be converted to int in %s on line %d
bool(false)
stringable class:
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, classWithToString given in %s on line %d

Warning: Object of class classWithToString could not be converted to int in %s on line %d
bool(false)
int castable class:
Warning: posix_ttyname(): Argument #1 ($file_descriptor) must be of type int|resource, GMP given in %s on line %d
bool(false)
