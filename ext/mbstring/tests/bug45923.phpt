--TEST--
Bug #45923 (mb_st[r]ripos() offset not handled correctly)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
mbstring.internal_encoding=UTF-8
--FILE--
<?php

function section($func, $haystack, $needle)
{
	echo "\n------- $func -----------\n\n";
	foreach(array(0, 3, 6, 9, 11, 12, -1, -3, -6, -20) as $offset) {
		echo "> Offset: $offset\n";
		var_dump($func($haystack,$needle,$offset));
	}
}

section('strpos'     , "abc abc abc"  , "abc");
section('mb_strpos'  , "●○◆ ●○◆ ●○◆", "●○◆");

section('stripos'    , "abc abc abc"  , "abc");
section('mb_stripos' , "●○◆ ●○◆ ●○◆", "●○◆");

section('strrpos'    , "abc abc abc"  , "abc");
section('mb_strrpos' , "●○◆ ●○◆ ●○◆", "●○◆");

section('strripos'   , "abc abc abc"  , "abc");
section('mb_strripos', "●○◆ ●○◆ ●○◆", "●○◆");
?>
--EXPECTF--
------- strpos -----------

> Offset: 0
int(0)
> Offset: 3
int(4)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20

Warning: strpos(): Offset not contained in string in %s on line %d
bool(false)

------- mb_strpos -----------

> Offset: 0
int(0)
> Offset: 3
int(4)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20

Warning: mb_strpos(): Offset not contained in string in %s on line %d
bool(false)

------- stripos -----------

> Offset: 0
int(0)
> Offset: 3
int(4)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20

Warning: stripos(): Offset not contained in string in %s on line %d
bool(false)

------- mb_stripos -----------

> Offset: 0
int(0)
> Offset: 3
int(4)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

------- strrpos -----------

> Offset: 0
int(8)
> Offset: 3
int(8)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20

Warning: strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

------- mb_strrpos -----------

> Offset: 0
int(8)
> Offset: 3
int(8)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20

Warning: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

------- strripos -----------

> Offset: 0
int(8)
> Offset: 3
int(8)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20

Warning: strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

------- mb_strripos -----------

> Offset: 0
int(8)
> Offset: 3
int(8)
> Offset: 6
int(8)
> Offset: 9
bool(false)
> Offset: 11
bool(false)
> Offset: 12

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)