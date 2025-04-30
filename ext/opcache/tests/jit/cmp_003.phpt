--TEST--
JIT CMP: 003 Comparison with NaN
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.protect_memory=1
--EXTENSIONS--
opcache
--FILE--
<?php
function t() {
    echo "!";
    return true;
}
function f() {
    echo "!";
    return false;
}
$a = 0.0;
$i = 0;
$b = NAN;
$c = true;
$d = false;
var_dump($a == $b);
var_dump($a != $b);
var_dump($a < $b);
var_dump($a > $b);
var_dump($a <= $b);
var_dump($a >= $b);
var_dump($a == $b ? 1 : 0);
var_dump($a != $b ? 1 : 0);
var_dump($a < $b ? 1 : 0);
var_dump($a > $b ? 1 : 0);
var_dump($a <= $b ? 1 : 0);
var_dump($a >= $b ? 1 : 0);
if ($a == $b) {
} else {
    echo "1\n";
}
if ($a != $b) {
} else {
    echo "2\n";
}
if ($a < $b) {
} else {
    echo "3\n";
}
if ($a > $b) {
} else {
    echo "4\n";
}
if ($a <= $b) {
} else {
    echo "5\n";
}
if ($a >= $b) {
} else {
    echo "6\n";
}
var_dump($i == $b ? 1 : 0);
var_dump($i != $b ? 1 : 0);
var_dump($i < $b ? 1 : 0);
var_dump($i > $b ? 1 : 0);
var_dump($i <= $b ? 1 : 0);
var_dump($i >= $b ? 1 : 0);
if ($i == $b) {
} else {
    echo "1\n";
}
if ($i != $b) {
} else {
    echo "2\n";
}
if ($i < $b) {
} else {
    echo "3\n";
}
if ($i > $b) {
} else {
    echo "4\n";
}
if ($i <= $b) {
} else {
    echo "5\n";
}
if ($i >= $b) {
} else {
    echo "6\n";
}
var_dump($a == $b && t());
var_dump($a != $b && t());
var_dump($a < $b && t());
var_dump($a > $b && t());
var_dump($a <= $b && t());
var_dump($a >= $b && t());
var_dump($a == $b || f());
var_dump($a != $b || f());
var_dump($a < $b || f());
var_dump($a > $b || f());
var_dump($a <= $b || f());
var_dump($a >= $b || f());
var_dump($i == $b && t());
var_dump($i != $b && t());
var_dump($i < $b && t());
var_dump($i > $b && t());
var_dump($i <= $b && t());
var_dump($i >= $b && t());
var_dump($i == $b || f());
var_dump($i != $b || f());
var_dump($i < $b || f());
var_dump($i > $b || f());
var_dump($i <= $b || f());
var_dump($i >= $b || f());
$a=NAN;
var_dump($a == $b);
var_dump($a != $b);
var_dump($a < $b);
var_dump($a > $b);
var_dump($a <= $b);
var_dump($a >= $b);
var_dump($a == $b ? 1 : 0);
var_dump($a != $b ? 1 : 0);
var_dump($a < $b ? 1 : 0);
var_dump($a > $b ? 1 : 0);
var_dump($a <= $b ? 1 : 0);
var_dump($a >= $b ? 1 : 0);
?>
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
1
3
4
5
6
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
1
3
4
5
6
bool(false)
!bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
!bool(false)
bool(true)
!bool(false)
!bool(false)
!bool(false)
!bool(false)
bool(false)
!bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
!bool(false)
bool(true)
!bool(false)
!bool(false)
!bool(false)
!bool(false)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
