--TEST--
Scalar type basics
--FILE--
<?php

$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_DEPRECATED => 'E_DEPRECATED'
];
set_error_handler(function (int $errno, string $errmsg, string $file, int $line) use ($errnames) {
    echo "$errnames[$errno]: $errmsg on line $line\n";
    return true;
});

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; }
];

class StringCapable implements Stringable {
    public function __toString() {
        return "foobar";
    }
}

$values = [
    1,
    "1",
    1.0,
    1.5,
    "1a",
    "a",
    "",
    PHP_INT_MAX,
    NAN,
    TRUE,
    FALSE,
    NULL,
    [],
    new StdClass,
    new StringCapable,
    fopen("data:text/plain,foobar", "r")
];

foreach ($functions as $type => $function) {
    echo PHP_EOL, "Testing '$type' type:", PHP_EOL;
    foreach ($values as $value) {
        echo PHP_EOL . "*** Trying ";
        var_dump($value);
        try {
            var_dump($function($value));
        } catch (\TypeError $e) {
            echo "*** Caught " . $e->getMessage() . PHP_EOL;
        }
    }
}
echo PHP_EOL . "Done";
?>
--EXPECTF--
Testing 'int' type:

*** Trying int(1)
int(1)

*** Trying string(1) "1"
int(1)

*** Trying float(1)
int(1)

*** Trying float(1.5)
E_DEPRECATED: Implicit conversion from float 1.5 to int loses precision on line 14
int(1)

*** Trying string(2) "1a"
*** Caught {closure}(): Argument #1 ($i) must be of type int, string given, called in %s on line %d

*** Trying string(1) "a"
*** Caught {closure}(): Argument #1 ($i) must be of type int, string given, called in %s on line %d

*** Trying string(0) ""
*** Caught {closure}(): Argument #1 ($i) must be of type int, string given, called in %s on line %d

*** Trying int(%d)
int(%d)

*** Trying float(NAN)
*** Caught {closure}(): Argument #1 ($i) must be of type int, float given, called in %s on line %d

*** Trying bool(true)
int(1)

*** Trying bool(false)
int(0)

*** Trying NULL
*** Caught {closure}(): Argument #1 ($i) must be of type int, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught {closure}(): Argument #1 ($i) must be of type int, array given, called in %s on line %d

*** Trying object(stdClass)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($i) must be of type int, stdClass given, called in %s on line %d

*** Trying object(StringCapable)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($i) must be of type int, StringCapable given, called in %s on line %d

*** Trying resource(%d) of type (stream)
*** Caught {closure}(): Argument #1 ($i) must be of type int, resource given, called in %s on line %d

Testing 'float' type:

*** Trying int(1)
float(1)

*** Trying string(1) "1"
float(1)

*** Trying float(1)
float(1)

*** Trying float(1.5)
float(1.5)

*** Trying string(2) "1a"
*** Caught {closure}(): Argument #1 ($f) must be of type float, string given, called in %s on line %d

*** Trying string(1) "a"
*** Caught {closure}(): Argument #1 ($f) must be of type float, string given, called in %s on line %d

*** Trying string(0) ""
*** Caught {closure}(): Argument #1 ($f) must be of type float, string given, called in %s on line %d

*** Trying int(%d)
float(%s)

*** Trying float(NAN)
float(NAN)

*** Trying bool(true)
float(1)

*** Trying bool(false)
float(0)

*** Trying NULL
*** Caught {closure}(): Argument #1 ($f) must be of type float, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught {closure}(): Argument #1 ($f) must be of type float, array given, called in %s on line %d

*** Trying object(stdClass)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($f) must be of type float, stdClass given, called in %s on line %d

*** Trying object(StringCapable)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($f) must be of type float, StringCapable given, called in %s on line %d

*** Trying resource(%d) of type (stream)
*** Caught {closure}(): Argument #1 ($f) must be of type float, resource given, called in %s on line %d

Testing 'string' type:

*** Trying int(1)
string(1) "1"

*** Trying string(1) "1"
string(1) "1"

*** Trying float(1)
string(1) "1"

*** Trying float(1.5)
string(3) "1.5"

*** Trying string(2) "1a"
string(2) "1a"

*** Trying string(1) "a"
string(1) "a"

*** Trying string(0) ""
string(0) ""

*** Trying int(%d)
string(%d) "%d"

*** Trying float(NAN)
string(3) "NAN"

*** Trying bool(true)
string(1) "1"

*** Trying bool(false)
string(0) ""

*** Trying NULL
*** Caught {closure}(): Argument #1 ($s) must be of type string, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught {closure}(): Argument #1 ($s) must be of type string, array given, called in %s on line %d

*** Trying object(stdClass)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($s) must be of type string, stdClass given, called in %s on line %d

*** Trying object(StringCapable)#%s (0) {
}
string(6) "foobar"

*** Trying resource(%d) of type (stream)
*** Caught {closure}(): Argument #1 ($s) must be of type string, resource given, called in %s on line %d

Testing 'bool' type:

*** Trying int(1)
bool(true)

*** Trying string(1) "1"
bool(true)

*** Trying float(1)
bool(true)

*** Trying float(1.5)
bool(true)

*** Trying string(2) "1a"
bool(true)

*** Trying string(1) "a"
bool(true)

*** Trying string(0) ""
bool(false)

*** Trying int(%d)
bool(true)

*** Trying float(NAN)
bool(true)

*** Trying bool(true)
bool(true)

*** Trying bool(false)
bool(false)

*** Trying NULL
*** Caught {closure}(): Argument #1 ($b) must be of type bool, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught {closure}(): Argument #1 ($b) must be of type bool, array given, called in %s on line %d

*** Trying object(stdClass)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($b) must be of type bool, stdClass given, called in %s on line %d

*** Trying object(StringCapable)#%s (0) {
}
*** Caught {closure}(): Argument #1 ($b) must be of type bool, StringCapable given, called in %s on line %d

*** Trying resource(%d) of type (stream)
*** Caught {closure}(): Argument #1 ($b) must be of type bool, resource given, called in %s on line %d

Done
