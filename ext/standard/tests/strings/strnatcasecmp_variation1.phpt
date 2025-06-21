--TEST--
Test strnatcasecmp() function : variation
--CREDITS--
Felix De Vliegher <felix.devliegher@gmail.com>
--FILE--
<?php

function str_dump($a, $b) {
    var_dump(strnatcasecmp($a, $b));
}

echo "*** Testing strnatcasecmp() : variation ***\n";

str_dump('0', '');
str_dump('fooBar', '');
str_dump('', '-1');
str_dump("Hello\0world", "Helloworld");
str_dump("\x0", "\0");

?>
--EXPECT--
*** Testing strnatcasecmp() : variation ***
int(1)
int(1)
int(-1)
int(-1)
int(0)
