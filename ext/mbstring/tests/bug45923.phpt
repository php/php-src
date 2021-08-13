--TEST--
Bug #45923 (mb_st[r]ripos() offset not handled correctly)
--EXTENSIONS--
mbstring
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
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
mb_strpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
mb_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
bool(false)
> Offset: -3
int(8)
> Offset: -6
int(8)
> Offset: -20
mb_stripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
mb_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
mb_strrpos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)

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
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
> Offset: -1
int(8)
> Offset: -3
int(8)
> Offset: -6
int(4)
> Offset: -20
mb_strripos(): Argument #3 ($offset) must be contained in argument #1 ($haystack)
