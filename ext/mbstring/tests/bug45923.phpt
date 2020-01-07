--TEST--
Bug #45923 (mb_st[r]ripos() offset not handled correctly)
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php

function section($func, $haystack, $needle)
{
	echo "\n------- $func -----------\n\n";
	foreach([0, 3, 6, 9, 11, 12, -1, -3, -6, -20] as $offset) {
		echo "> Offset: $offset\n";
        try {
		    var_dump($func($haystack, $needle, $offset));
        } catch (\ValueError $e) {
            echo $e->getMessage() . \PHP_EOL;
        }
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
--EXPECT--
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
Offset not contained in string
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
Offset not contained in string

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
Offset not contained in string
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
Offset not contained in string
