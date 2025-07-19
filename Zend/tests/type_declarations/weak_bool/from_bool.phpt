--TEST--
Check deprecation from bool type in weak mode
--FILE--
<?php

echo "int:\n";
function fpi(int $v) {
    var_dump($v);
}

function fri($v): int {
    var_dump($v);
    return $v;
}

class CI {
    public int $v;
}
$o = new CI();

fpi(true);
fri(true);
$o->v = true;
var_dump($o->v);

echo "float:\n";
function fpf(float $v) {
    var_dump($v);
}

function frf($v): float {
    var_dump($v);
    return $v;
}

class CF {
    public float $v;
}
$o = new CF();

fpf(true);
frf(true);
$o->v = true;
var_dump($o->v);

echo "string:\n";
function fps(string $v) {
    var_dump($v);
}

function frs($v): string {
    var_dump($v);
    return $v;
}

class CS {
    public string $v;
}
$o = new CS();

fps(true);
frs(true);
$o->v = true;
var_dump($o->v);

?>
--EXPECTF--
int:

Deprecated: fpi(): Passing bool to parameter #1 ($v) of type int is deprecated in %s on line %d
int(1)
bool(true)

Deprecated: fri(): Returning type bool which is implicitly converted to type int is deprecated in %s on line %d

Deprecated: Assigning bool to typed property which is implicitly converted to type int is deprecated in %s on line %d
int(1)
float:

Deprecated: fpf(): Passing bool to parameter #1 ($v) of type float is deprecated in %s on line %d
float(1)
bool(true)

Deprecated: frf(): Returning type bool which is implicitly converted to type float is deprecated in %s on line %d

Deprecated: Assigning bool to typed property which is implicitly converted to type float is deprecated in %s on line %d
float(1)
string:

Deprecated: fps(): Passing bool to parameter #1 ($v) of type string is deprecated in %s on line %d
string(1) "1"
bool(true)

Deprecated: frs(): Returning type bool which is implicitly converted to type string is deprecated in %s on line %d

Deprecated: Assigning bool to typed property which is implicitly converted to type string is deprecated in %s on line %d
string(1) "1"
