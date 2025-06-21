--TEST--
Test implode() function with strings with null bytes
--FILE--
<?php

$pieces = [
    'PHP',
    'is',
    'great',
];

$glues = [
    "\0",
    "string\x00between",
];

foreach ($glues as $glue) {
    var_dump(implode($glue, $pieces));
}

?>
--EXPECTF--
string(12) "PHP%0is%0great"
string(38) "PHPstring%0betweenisstring%0betweengreat"
