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

try {
	$a = [];
	$a .= "foo";
} catch (Throwable $e) { var_dump($a); }

try {
	$a = "foo";
	$a .= [];
} catch (Throwable $e) { var_dump($a); }

$x = new stdClass;
try { $x += 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x += new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x -= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x -= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x *= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x *= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x /= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x /= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x %= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x %= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x **= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x **= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x ^= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x ^= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x &= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x &= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x |= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x |= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x <<= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x <<= new stdClass; }
catch (Exception $e) {}
var_dump($x);

$x = new stdClass;
try { $x >>= 1; }
catch (Exception $e) {}
var_dump($x);

$x = 1;
try { $x >>= new stdClass; }
catch (Exception $e) {}
var_dump($x);
?>
--EXPECTF--
int(1)
int(1)
int(1)
array(0) {
}
string(3) "foo"
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
object(stdClass)#%d (0) {
}
int(1)
