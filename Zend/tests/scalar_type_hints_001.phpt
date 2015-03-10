--TEST--
Scalar type hints 001
--FILE--
<?php
function error($code, $msg) {
	$GLOBALS["error"] = "($code): $msg";
}
set_error_handler('error');
function bool_func(bool $x) {
	if (isset($GLOBALS["error"])) {
		echo $GLOBALS["error"] . "\n";
		unset($GLOBALS["error"]);
	} else {
		var_dump($x);
	}
}
function check_bool($x) {
	var_dump($x);
	try {
		bool_func($x);
	} catch (EngineException $e) {
		error($e->getCode(), $e->getMessage());
		if (isset($GLOBALS["error"])) {
			echo $GLOBALS["error"] . "\n";
			unset($GLOBALS["error"]);
		}
	}
	echo "----\n";
}
function int_func(int $x) {
	if (isset($GLOBALS["error"])) {
		echo $GLOBALS["error"] . "\n";
		unset($GLOBALS["error"]);
	} else {
		var_dump($x);
	}
}
function check_int($x) {
	var_dump($x);
	try {
		int_func($x);
	} catch (EngineException $e) {
		error($e->getCode(), $e->getMessage());
		if (isset($GLOBALS["error"])) {
			echo $GLOBALS["error"] . "\n";
			unset($GLOBALS["error"]);
		}
	}
	echo "----\n";
}
function float_func(float $x) {
	if (isset($GLOBALS["error"])) {
		echo $GLOBALS["error"] . "\n";
		unset($GLOBALS["error"]);
	} else {
		var_dump($x);
	}
}
function check_float($x) {
	var_dump($x);
	try {
		float_func($x);
	} catch (EngineException $e) {
		error($e->getCode(), $e->getMessage());
		if (isset($GLOBALS["error"])) {
			echo $GLOBALS["error"] . "\n";
			unset($GLOBALS["error"]);
		}
	}
	echo "----\n";
}
function string_func(string $x) {
	if (isset($GLOBALS["error"])) {
		echo $GLOBALS["error"] . "\n";
		unset($GLOBALS["error"]);
	} else {
		var_dump($x);
	}
}
function check_string($x) {
	var_dump($x);
	try {
		string_func($x);
	} catch (EngineException $e) {
		error($e->getCode(), $e->getMessage());
		if (isset($GLOBALS["error"])) {
			echo $GLOBALS["error"] . "\n";
			unset($GLOBALS["error"]);
		}
	}
	echo "----\n";
}

echo "== bool ==\n";
check_bool(null);
check_bool(false);
check_bool(true);
check_bool(0);
check_bool(1);
check_bool(2);
check_bool(0.0);
check_bool(1.0);
check_bool(0.2);
check_bool("");
check_bool("0");
check_bool("1");

echo "== int ==\n";
check_int(null);
check_int(false);
check_int(true);
check_int(0);
check_int(1);
check_int(0.0);
check_int(1.0);
check_int(0.2);
check_int("");
check_int("0");
check_int("1");
check_int(" 1");
check_int("1 ");
check_int("1111111111111111111111111111111111111");
check_int("1.0");
check_int("1.1");

echo "== float ==\n";
check_float(null);
check_float(false);
check_float(true);
check_float(0);
check_float(1);
check_float(0.0);
check_float(1.1);
check_float("");
check_float("0");
check_float("1");
check_float(" 1");
check_float("1 ");
check_float("1111111111111111111111111111111111111");
check_float("1.0");
check_float("1.1");

echo "== string ==\n";
check_string(null);
check_string(false);
check_string(true);
check_string(0);
check_string(1);
check_string(0.0);
check_string(1.0);
check_string(1.1);
?>
--EXPECTF--
== bool ==
NULL
(1): Argument 1 passed to bool_func() must be of the type boolean, null given, called in %s on line %d and defined
----
bool(false)
bool(false)
----
bool(true)
bool(true)
----
int(0)
bool(false)
----
int(1)
bool(true)
----
int(2)
bool(true)
----
float(0)
(1): Argument 1 passed to bool_func() must be of the type boolean, float given, called in %s on line %d and defined
----
float(1)
(1): Argument 1 passed to bool_func() must be of the type boolean, float given, called in %s on line %d and defined
----
float(0.2)
(1): Argument 1 passed to bool_func() must be of the type boolean, float given, called in %s on line %d and defined
----
string(0) ""
bool(false)
----
string(1) "0"
bool(false)
----
string(1) "1"
bool(true)
----
== int ==
NULL
(1): Argument 1 passed to int_func() must be of the type integer, null given, called in %s on line %d and defined
----
bool(false)
(1): Argument 1 passed to int_func() must be of the type integer, boolean given, called in %s on line %d and defined
----
bool(true)
(1): Argument 1 passed to int_func() must be of the type integer, boolean given, called in %s on line %d and defined
----
int(0)
int(0)
----
int(1)
int(1)
----
float(0)
int(0)
----
float(1)
int(1)
----
float(0.2)
(1): Argument 1 passed to int_func() must be of the type integer, float given, called in %s on line %d and defined
----
string(0) ""
(1): Argument 1 passed to int_func() must be of the type integer, string given, called in %s on line %d and defined
----
string(1) "0"
int(0)
----
string(1) "1"
int(1)
----
string(2) " 1"
int(1)
----
string(2) "1 "
(8): A non well formed numeric value encountered
----
string(37) "1111111111111111111111111111111111111"
(1): Argument 1 passed to int_func() must be of the type integer, string given, called in %s on line %d and defined
----
string(3) "1.0"
int(1)
----
string(3) "1.1"
(1): Argument 1 passed to int_func() must be of the type integer, string given, called in %s on line %d and defined
----
== float ==
NULL
(1): Argument 1 passed to float_func() must be of the type float, null given, called in %s on line %d and defined
----
bool(false)
(1): Argument 1 passed to float_func() must be of the type float, boolean given, called in %s on line %d and defined
----
bool(true)
(1): Argument 1 passed to float_func() must be of the type float, boolean given, called in %s on line %d and defined
----
int(0)
float(0)
----
int(1)
float(1)
----
float(0)
float(0)
----
float(1.1)
float(1.1)
----
string(0) ""
(1): Argument 1 passed to float_func() must be of the type float, string given, called in %s on line %d and defined
----
string(1) "0"
float(0)
----
string(1) "1"
float(1)
----
string(2) " 1"
float(1)
----
string(2) "1 "
(8): A non well formed numeric value encountered
----
string(37) "1111111111111111111111111111111111111"
float(1.1111111111111E+36)
----
string(3) "1.0"
float(1)
----
string(3) "1.1"
float(1.1)
----
== string ==
NULL
(1): Argument 1 passed to string_func() must be of the type string, null given, called in %s on line %d and defined
----
bool(false)
(1): Argument 1 passed to string_func() must be of the type string, boolean given, called in %s on line %d and defined
----
bool(true)
(1): Argument 1 passed to string_func() must be of the type string, boolean given, called in %s on line %d and defined
----
int(0)
string(1) "0"
----
int(1)
string(1) "1"
----
float(0)
string(1) "0"
----
float(1)
string(1) "1"
----
float(1.1)
string(3) "1.1"
----
