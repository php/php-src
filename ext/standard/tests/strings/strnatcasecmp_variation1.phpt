--TEST--
Test strnatcasecmp() function : variation
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php
/* Preparation */
class a
{
    function __toString()
    {
        return "Hello WORLD";
    }
}

class b
{
    function __toString()
    {
        return "HELLO world";
    }
}

$a = new a();
$b = new b();

function str_dump($a, $b) {
    var_dump(strnatcasecmp($a, $b));
}

echo "*** Testing strnatcasecmp() : variation ***\n";

str_dump('0', false);
str_dump('fooBar', '');
str_dump('', -1);
str_dump("Hello\0world", "Helloworld");
str_dump("\x0", "\0");
str_dump($a, $b);

?>
--EXPECTF--
*** Testing strnatcasecmp() : variation ***

Deprecated: Implicit bool to string coercion is deprecated in %s on line %d
int(1)
int(1)
int(-1)
int(-1)
int(0)
int(0)
