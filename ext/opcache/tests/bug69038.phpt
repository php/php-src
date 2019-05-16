--TEST--
Bug #69038 (switch(SOMECONSTANT) misbehaves)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function a($a = "bad") {

	switch (PHP_OS) {
	case "LALALALA" : return "LALALAL";
	case PHP_OS: return "okey";
	default:   break;
	}

	return $a;
}

var_dump(a());


function b($b = "bad") {
	switch (PHP_OS) {
	case "LALALAL": return "bad";
	case PHP_OS:
		switch (PHP_OS) {
		case "FOO": break;
		case PHP_OS: return "okey";
		default :
			break;
		}
		break;
	default:
		break;
	}
	return $b;
}
var_dump(b());

function c($b = "bad") {
	switch (extension_loaded("standard")) {
	case 0 : return "LALALAL";
	case 1 : return "okey";
	default : return "bad";
	}
}
var_dump(c());

function d() {
	switch (PHP_OS) {
		default: return "bad";
		case PHP_OS: return "okey";
	}
}

var_dump(d());
?>
--EXPECT--
string(4) "okey"
string(4) "okey"
string(4) "okey"
string(4) "okey"
