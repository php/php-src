--TEST--
Behavior of failing compound assignment
--INI--
opcache.optimization_level=0
--FILE--
<?php

try {
    $a = 1;
    $a %= 0;
} catch (Error $e) { var_dump($a); }

try {
    $a = 1;
    $a >>= -1;
} catch (Error $e) { var_dump($a); }

try {
    $a = 1;
    $a <<= -1;
} catch (Error $e) { var_dump($a); }

set_error_handler(function($type, $msg) { throw new Exception($msg); });

function concat(&$a, $b) { $a .= $b; }

try {
    $a = [];
    concat($a, "foo");
} catch (Throwable $e) { var_dump($a); }

try {
    $a = "foo";
    concat($a, []);
} catch (Throwable $e) { var_dump($a); }

$x = new stdClass;
try { $x += 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x += new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x += new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x -= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x -= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x -= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x *= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x *= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x *= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x /= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x /= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x /= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x %= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x %= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x %= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x **= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x **= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x **= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x ^= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x ^= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x ^= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x &= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x &= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x &= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x |= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x |= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x |= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x <<= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x <<= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x <<= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = new stdClass;
try { $x >>= 1; }
catch (Throwable $e) {}
var_dump($x);

$x = 1;
try { $x >>= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

$x = "foo";
try { $x >>= new stdClass; }
catch (Throwable $e) {}
var_dump($x);

?>
--EXPECTF--
int(1)
int(1)
int(1)
string(8) "Arrayfoo"
string(8) "fooArray"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
string(3) "foo"
