--TEST--
Scalar type strict mode
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php
declare(strict_types=1);

$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_RECOVERABLE_ERROR => 'E_RECOVERABLE_ERROR'
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

class Stringable {
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
    new Stringable,
    fopen("data:text/plain,foobar", "r")
];

foreach ($functions as $type => $function) {
    echo PHP_EOL, "Testing '$type' type:", PHP_EOL;
    foreach ($values as $value) {
        echo PHP_EOL . "*** Trying ";
        var_dump($value);
        try {
            var_dump($function($value));
        } catch (TypeError $e) {
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
*** Caught Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line %d

*** Trying float(1)
*** Caught Argument 1 passed to {closure}() must be of the type integer, float given, called in %s on line %d

*** Trying float(1.5)
*** Caught Argument 1 passed to {closure}() must be of the type integer, float given, called in %s on line %d

*** Trying string(2) "1a"
*** Caught Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line %d

*** Trying string(1) "a"
*** Caught Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line %d

*** Trying string(0) ""
*** Caught Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line %d

*** Trying int(2147483647)
int(2147483647)

*** Trying float(NAN)
*** Caught Argument 1 passed to {closure}() must be of the type integer, float given, called in %s on line %d

*** Trying bool(true)
*** Caught Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line %d

*** Trying bool(false)
*** Caught Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line %d

*** Trying NULL
*** Caught Argument 1 passed to {closure}() must be of the type integer, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type integer, array given, called in %s on line %d

*** Trying object(stdClass)#6 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type integer, object given, called in %s on line %d

*** Trying object(Stringable)#7 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type integer, object given, called in %s on line %d

*** Trying resource(5) of type (stream)
*** Caught Argument 1 passed to {closure}() must be of the type integer, resource given, called in %s on line %d

Testing 'float' type:

*** Trying int(1)
float(1)

*** Trying string(1) "1"
*** Caught Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line %d

*** Trying float(1)
float(1)

*** Trying float(1.5)
float(1.5)

*** Trying string(2) "1a"
*** Caught Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line %d

*** Trying string(1) "a"
*** Caught Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line %d

*** Trying string(0) ""
*** Caught Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line %d

*** Trying int(2147483647)
float(2147483647)

*** Trying float(NAN)
float(NAN)

*** Trying bool(true)
*** Caught Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line %d

*** Trying bool(false)
*** Caught Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line %d

*** Trying NULL
*** Caught Argument 1 passed to {closure}() must be of the type float, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type float, array given, called in %s on line %d

*** Trying object(stdClass)#6 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type float, object given, called in %s on line %d

*** Trying object(Stringable)#7 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type float, object given, called in %s on line %d

*** Trying resource(5) of type (stream)
*** Caught Argument 1 passed to {closure}() must be of the type float, resource given, called in %s on line %d

Testing 'string' type:

*** Trying int(1)
*** Caught Argument 1 passed to {closure}() must be of the type string, integer given, called in %s on line %d

*** Trying string(1) "1"
string(1) "1"

*** Trying float(1)
*** Caught Argument 1 passed to {closure}() must be of the type string, float given, called in %s on line %d

*** Trying float(1.5)
*** Caught Argument 1 passed to {closure}() must be of the type string, float given, called in %s on line %d

*** Trying string(2) "1a"
string(2) "1a"

*** Trying string(1) "a"
string(1) "a"

*** Trying string(0) ""
string(0) ""

*** Trying int(2147483647)
*** Caught Argument 1 passed to {closure}() must be of the type string, integer given, called in %s on line %d

*** Trying float(NAN)
*** Caught Argument 1 passed to {closure}() must be of the type string, float given, called in %s on line %d

*** Trying bool(true)
*** Caught Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line %d

*** Trying bool(false)
*** Caught Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line %d

*** Trying NULL
*** Caught Argument 1 passed to {closure}() must be of the type string, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type string, array given, called in %s on line %d

*** Trying object(stdClass)#6 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type string, object given, called in %s on line %d

*** Trying object(Stringable)#7 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type string, object given, called in %s on line %d

*** Trying resource(5) of type (stream)
*** Caught Argument 1 passed to {closure}() must be of the type string, resource given, called in %s on line %d

Testing 'bool' type:

*** Trying int(1)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, integer given, called in %s on line %d

*** Trying string(1) "1"
*** Caught Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line %d

*** Trying float(1)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, float given, called in %s on line %d

*** Trying float(1.5)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, float given, called in %s on line %d

*** Trying string(2) "1a"
*** Caught Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line %d

*** Trying string(1) "a"
*** Caught Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line %d

*** Trying string(0) ""
*** Caught Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line %d

*** Trying int(2147483647)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, integer given, called in %s on line %d

*** Trying float(NAN)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, float given, called in %s on line %d

*** Trying bool(true)
bool(true)

*** Trying bool(false)
bool(false)

*** Trying NULL
*** Caught Argument 1 passed to {closure}() must be of the type boolean, null given, called in %s on line %d

*** Trying array(0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type boolean, array given, called in %s on line %d

*** Trying object(stdClass)#6 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type boolean, object given, called in %s on line %d

*** Trying object(Stringable)#7 (0) {
}
*** Caught Argument 1 passed to {closure}() must be of the type boolean, object given, called in %s on line %d

*** Trying resource(5) of type (stream)
*** Caught Argument 1 passed to {closure}() must be of the type boolean, resource given, called in %s on line %d

Done
