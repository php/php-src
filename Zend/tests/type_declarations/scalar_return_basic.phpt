--TEST--
Return scalar type basics
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only"); ?>
--FILE--
<?php

$errnames = [
    E_NOTICE => 'E_NOTICE',
    E_WARNING => 'E_WARNING',
    E_DEPRECATED => 'E_DEPRECATED',
];
set_error_handler(function (int $errno, string $errmsg, string $file, int $line) use ($errnames) {
    echo "$errnames[$errno]: $errmsg on line $line\n";
    return true;
});

$functions = [
    'int' => function ($i): int { return $i; },
    'float' => function ($f): float { return $f; },
    'string' => function ($s): string { return $s; },
    'bool' => function ($b): bool { return $b; }
];

class StringCapable {
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
        echo "*** Trying ";
        var_dump($value);
        try {
            var_dump($function($value));
        } catch (TypeError $e) {
            echo "*** Caught ", $e->getMessage(), " in ", $e->getFile(), " on line ", $e->getLine(), PHP_EOL;
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
E_DEPRECATED: Implicit conversion from float 1.5 to int loses precision on line %d
int(1)
*** Trying string(2) "1a"
*** Caught {closure}(): Return value must be of type int, string returned in %s on line %d
*** Trying string(1) "a"
*** Caught {closure}(): Return value must be of type int, string returned in %s on line %d
*** Trying string(0) ""
*** Caught {closure}(): Return value must be of type int, string returned in %s on line %d
*** Trying int(2147483647)
int(2147483647)
*** Trying float(NAN)
*** Caught {closure}(): Return value must be of type int, float returned in %s on line %d
*** Trying bool(true)
int(1)
*** Trying bool(false)
int(0)
*** Trying NULL
*** Caught {closure}(): Return value must be of type int, null returned in %s on line %d
*** Trying array(0) {
}
*** Caught {closure}(): Return value must be of type int, array returned in %s on line %d
*** Trying object(stdClass)#6 (0) {
}
*** Caught {closure}(): Return value must be of type int, stdClass returned in %s on line %d
*** Trying object(StringCapable)#7 (0) {
}
*** Caught {closure}(): Return value must be of type int, StringCapable returned in %s on line %d
*** Trying resource(5) of type (stream)
*** Caught {closure}(): Return value must be of type int, resource returned in %s on line %d

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
*** Caught {closure}(): Return value must be of type float, string returned in %s on line %d
*** Trying string(1) "a"
*** Caught {closure}(): Return value must be of type float, string returned in %s on line %d
*** Trying string(0) ""
*** Caught {closure}(): Return value must be of type float, string returned in %s on line %d
*** Trying int(2147483647)
float(2147483647)
*** Trying float(NAN)
float(NAN)
*** Trying bool(true)
float(1)
*** Trying bool(false)
float(0)
*** Trying NULL
*** Caught {closure}(): Return value must be of type float, null returned in %s on line %d
*** Trying array(0) {
}
*** Caught {closure}(): Return value must be of type float, array returned in %s on line %d
*** Trying object(stdClass)#6 (0) {
}
*** Caught {closure}(): Return value must be of type float, stdClass returned in %s on line %d
*** Trying object(StringCapable)#7 (0) {
}
*** Caught {closure}(): Return value must be of type float, StringCapable returned in %s on line %d
*** Trying resource(5) of type (stream)
*** Caught {closure}(): Return value must be of type float, resource returned in %s on line %d

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
*** Trying int(2147483647)
string(10) "2147483647"
*** Trying float(NAN)
string(3) "NAN"
*** Trying bool(true)
string(1) "1"
*** Trying bool(false)
string(0) ""
*** Trying NULL
*** Caught {closure}(): Return value must be of type string, null returned in %s on line %d
*** Trying array(0) {
}
*** Caught {closure}(): Return value must be of type string, array returned in %s on line %d
*** Trying object(stdClass)#6 (0) {
}
*** Caught {closure}(): Return value must be of type string, stdClass returned in %s on line %d
*** Trying object(StringCapable)#7 (0) {
}
string(6) "foobar"
*** Trying resource(5) of type (stream)
*** Caught {closure}(): Return value must be of type string, resource returned in %s on line %d

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
*** Trying int(2147483647)
bool(true)
*** Trying float(NAN)
bool(true)
*** Trying bool(true)
bool(true)
*** Trying bool(false)
bool(false)
*** Trying NULL
*** Caught {closure}(): Return value must be of type bool, null returned in %s on line %d
*** Trying array(0) {
}
*** Caught {closure}(): Return value must be of type bool, array returned in %s on line %d
*** Trying object(stdClass)#6 (0) {
}
*** Caught {closure}(): Return value must be of type bool, stdClass returned in %s on line %d
*** Trying object(StringCapable)#7 (0) {
}
*** Caught {closure}(): Return value must be of type bool, StringCapable returned in %s on line %d
*** Trying resource(5) of type (stream)
*** Caught {closure}(): Return value must be of type bool, resource returned in %s on line %d

Done
