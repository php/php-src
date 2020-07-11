--TEST--
ReflectionMethod::getDocComment()
--INI--
opcache.save_comments=1
--FILE--
<?php

class X {
    /**
     * doc 1
     */
    // Some comment
    public
        $x = "x",
        $y = 'y',
        /** doc 2 */
        $z = 'z'
    ;
}

$reflection = new ReflectionProperty('\X', 'x');
echo 'X::x', PHP_EOL;
var_dump($reflection->getDocComment());

$reflection = new ReflectionProperty('\X', 'y');
echo 'X::y', PHP_EOL;
var_dump($reflection->getDocComment());

$reflection = new ReflectionProperty('\X', 'z');
echo 'X::z', PHP_EOL;
var_dump($reflection->getDocComment());
?>
--EXPECT--
X::x
string(24) "/**
     * doc 1
     */"
X::y
bool(false)
X::z
string(12) "/** doc 2 */"
