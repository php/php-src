--TEST--
Strict scalar type basics
--FILE--
<?php

declare(strict_types=1);

$functions = [
    'int' => function (int $i) { return $i; },
    'float' => function (float $f) { return $f; },
    'string' => function (string $s) { return $s; },
    'bool' => function (bool $b) { return $b; }
];

$values = [
    1,
    1.0,
    "1",
    TRUE,
    FALSE,
    NULL,
    [],
    new StdClass,
    fopen("data:text/plain,foobar", "r")
];

function type($value) {
    if (is_float($value)) {
        return "float";
    } else if (is_bool($value)) {
        return $value ? "true" : "false";
    } else if (is_null($value)) {
        return "null";
    } else {
        return gettype($value);
    }
}

foreach ($functions as $type => $function) {
    echo PHP_EOL, "Testing '$type' type:", PHP_EOL;
    foreach ($values as $value) {
        $errored = false;
        echo PHP_EOL . "*** Trying ", type($value), " value", PHP_EOL;
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

*** Trying integer value
int(1)

*** Trying float value
*** Caught {closure}(): Argument #1 ($i) must be of type int, float given, called in %s on line %d

*** Trying string value
*** Caught {closure}(): Argument #1 ($i) must be of type int, string given, called in %s on line %d

*** Trying true value
*** Caught {closure}(): Argument #1 ($i) must be of type int, bool given, called in %s on line %d

*** Trying false value
*** Caught {closure}(): Argument #1 ($i) must be of type int, bool given, called in %s on line %d

*** Trying null value
*** Caught {closure}(): Argument #1 ($i) must be of type int, null given, called in %s on line %d

*** Trying array value
*** Caught {closure}(): Argument #1 ($i) must be of type int, array given, called in %s on line %d

*** Trying object value
*** Caught {closure}(): Argument #1 ($i) must be of type int, stdClass given, called in %s on line %d

*** Trying resource value
*** Caught {closure}(): Argument #1 ($i) must be of type int, resource given, called in %s on line %d

Testing 'float' type:

*** Trying integer value
float(1)

*** Trying float value
float(1)

*** Trying string value
*** Caught {closure}(): Argument #1 ($f) must be of type float, string given, called in %s on line %d

*** Trying true value
*** Caught {closure}(): Argument #1 ($f) must be of type float, bool given, called in %s on line %d

*** Trying false value
*** Caught {closure}(): Argument #1 ($f) must be of type float, bool given, called in %s on line %d

*** Trying null value
*** Caught {closure}(): Argument #1 ($f) must be of type float, null given, called in %s on line %d

*** Trying array value
*** Caught {closure}(): Argument #1 ($f) must be of type float, array given, called in %s on line %d

*** Trying object value
*** Caught {closure}(): Argument #1 ($f) must be of type float, stdClass given, called in %s on line %d

*** Trying resource value
*** Caught {closure}(): Argument #1 ($f) must be of type float, resource given, called in %s on line %d

Testing 'string' type:

*** Trying integer value
*** Caught {closure}(): Argument #1 ($s) must be of type string, int given, called in %s on line %d

*** Trying float value
*** Caught {closure}(): Argument #1 ($s) must be of type string, float given, called in %s on line %d

*** Trying string value
string(1) "1"

*** Trying true value
*** Caught {closure}(): Argument #1 ($s) must be of type string, bool given, called in %s on line %d

*** Trying false value
*** Caught {closure}(): Argument #1 ($s) must be of type string, bool given, called in %s on line %d

*** Trying null value
*** Caught {closure}(): Argument #1 ($s) must be of type string, null given, called in %s on line %d

*** Trying array value
*** Caught {closure}(): Argument #1 ($s) must be of type string, array given, called in %s on line %d

*** Trying object value
*** Caught {closure}(): Argument #1 ($s) must be of type string, stdClass given, called in %s on line %d

*** Trying resource value
*** Caught {closure}(): Argument #1 ($s) must be of type string, resource given, called in %s on line %d

Testing 'bool' type:

*** Trying integer value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, int given, called in %s on line %d

*** Trying float value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, float given, called in %s on line %d

*** Trying string value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, string given, called in %s on line %d

*** Trying true value
bool(true)

*** Trying false value
bool(false)

*** Trying null value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, null given, called in %s on line %d

*** Trying array value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, array given, called in %s on line %d

*** Trying object value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, stdClass given, called in %s on line %d

*** Trying resource value
*** Caught {closure}(): Argument #1 ($b) must be of type bool, resource given, called in %s on line %d

Done
