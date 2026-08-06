--TEST--
Bug GH-23082: Restoring unserialize_callback_func to an empty string
--INI--
display_errors=1
--FILE--
<?php

function my_callback($name)
{
    echo "callback fired for $name\n";
}

$prev = ini_set('unserialize_callback_func', 'my_callback');
var_dump($prev);

var_dump(ini_set('unserialize_callback_func', $prev));
var_dump(ini_get('unserialize_callback_func'));

unserialize('O:20:"SomeNotExistingClass":0:{}');

?>
--EXPECT--
string(0) ""
string(11) "my_callback"
string(0) ""
