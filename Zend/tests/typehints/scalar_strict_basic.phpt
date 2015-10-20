--TEST--
Strict scalar type basics
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
*** Caught Argument 1 passed to {closure}() must be of the type integer, float given, called in %s on line %d

*** Trying string value
*** Caught Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line %d

*** Trying true value
*** Caught Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line %d

*** Trying false value
*** Caught Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line %d

*** Trying null value
*** Caught Argument 1 passed to {closure}() must be of the type integer, null given, called in %s on line %d

*** Trying array value
*** Caught Argument 1 passed to {closure}() must be of the type integer, array given, called in %s on line %d

*** Trying object value
*** Caught Argument 1 passed to {closure}() must be of the type integer, object given, called in %s on line %d

*** Trying resource value
*** Caught Argument 1 passed to {closure}() must be of the type integer, resource given, called in %s on line %d

Testing 'float' type:

*** Trying integer value
float(1)

*** Trying float value
float(1)

*** Trying string value
*** Caught Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line %d

*** Trying true value
*** Caught Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line %d

*** Trying false value
*** Caught Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line %d

*** Trying null value
*** Caught Argument 1 passed to {closure}() must be of the type float, null given, called in %s on line %d

*** Trying array value
*** Caught Argument 1 passed to {closure}() must be of the type float, array given, called in %s on line %d

*** Trying object value
*** Caught Argument 1 passed to {closure}() must be of the type float, object given, called in %s on line %d

*** Trying resource value
*** Caught Argument 1 passed to {closure}() must be of the type float, resource given, called in %s on line %d

Testing 'string' type:

*** Trying integer value
*** Caught Argument 1 passed to {closure}() must be of the type string, integer given, called in %s on line %d

*** Trying float value
*** Caught Argument 1 passed to {closure}() must be of the type string, float given, called in %s on line %d

*** Trying string value
string(1) "1"

*** Trying true value
*** Caught Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line %d

*** Trying false value
*** Caught Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line %d

*** Trying null value
*** Caught Argument 1 passed to {closure}() must be of the type string, null given, called in %s on line %d

*** Trying array value
*** Caught Argument 1 passed to {closure}() must be of the type string, array given, called in %s on line %d

*** Trying object value
*** Caught Argument 1 passed to {closure}() must be of the type string, object given, called in %s on line %d

*** Trying resource value
*** Caught Argument 1 passed to {closure}() must be of the type string, resource given, called in %s on line %d

Testing 'bool' type:

*** Trying integer value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, integer given, called in %s on line %d

*** Trying float value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, float given, called in %s on line %d

*** Trying string value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line %d

*** Trying true value
bool(true)

*** Trying false value
bool(false)

*** Trying null value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, null given, called in %s on line %d

*** Trying array value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, array given, called in %s on line %d

*** Trying object value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, object given, called in %s on line %d

*** Trying resource value
*** Caught Argument 1 passed to {closure}() must be of the type boolean, resource given, called in %s on line %d

Done
