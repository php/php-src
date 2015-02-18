--TEST--
Strict scalar type hint basics
--FILE--
<?php

declare(strict_types=1);

$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_RECOVERABLE_ERROR => 'E_RECOVERABLE_ERROR'
];
$errored = true;
set_error_handler(function (int $errno, string $errmsg, string $file, int $line) use ($errnames, &$errored) {
    echo "$errnames[$errno]: $errmsg on line $line\n";
    $errored = true;
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
    echo PHP_EOL, "Testing '$type' typehint:", PHP_EOL;
    foreach ($values as $value) {
        $errored = false;
        echo "*** Trying ", type($value), " value", PHP_EOL;
        $result = $function($value);
        if (!$errored) {
            var_dump($result);
        }
    }
}
--EXPECTF--

Testing 'int' typehint:
*** Trying integer value
int(1)
*** Trying float value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, float given, called in %s on line 53 and defined on line 18
*** Trying string value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, string given, called in %s on line 53 and defined on line 18
*** Trying true value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line 53 and defined on line 18
*** Trying false value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, boolean given, called in %s on line 53 and defined on line 18
*** Trying null value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, null given, called in %s on line 53 and defined on line 18
*** Trying array value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, array given, called in %s on line 53 and defined on line 18
*** Trying object value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, object given, called in %s on line 53 and defined on line 18
*** Trying resource value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type integer, resource given, called in %s on line 53 and defined on line 18

Testing 'float' typehint:
*** Trying integer value
float(1)
*** Trying float value
float(1)
*** Trying string value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, string given, called in %s on line 53 and defined on line 19
*** Trying true value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line 53 and defined on line 19
*** Trying false value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, boolean given, called in %s on line 53 and defined on line 19
*** Trying null value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, null given, called in %s on line 53 and defined on line 19
*** Trying array value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, array given, called in %s on line 53 and defined on line 19
*** Trying object value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, object given, called in %s on line 53 and defined on line 19
*** Trying resource value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type float, resource given, called in %s on line 53 and defined on line 19

Testing 'string' typehint:
*** Trying integer value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, integer given, called in %s on line 53 and defined on line 20
*** Trying float value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, float given, called in %s on line 53 and defined on line 20
*** Trying string value
string(1) "1"
*** Trying true value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line 53 and defined on line 20
*** Trying false value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, boolean given, called in %s on line 53 and defined on line 20
*** Trying null value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, null given, called in %s on line 53 and defined on line 20
*** Trying array value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, array given, called in %s on line 53 and defined on line 20
*** Trying object value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, object given, called in %s on line 53 and defined on line 20
*** Trying resource value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type string, resource given, called in %s on line 53 and defined on line 20

Testing 'bool' typehint:
*** Trying integer value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, integer given, called in %s on line 53 and defined on line 21
*** Trying float value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, float given, called in %s on line 53 and defined on line 21
*** Trying string value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, string given, called in %s on line 53 and defined on line 21
*** Trying true value
bool(true)
*** Trying false value
bool(false)
*** Trying null value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, null given, called in %s on line 53 and defined on line 21
*** Trying array value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, array given, called in %s on line 53 and defined on line 21
*** Trying object value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, object given, called in %s on line 53 and defined on line 21
*** Trying resource value
E_RECOVERABLE_ERROR: Argument 1 passed to {closure}() must be of the type boolean, resource given, called in %s on line 53 and defined on line 21
