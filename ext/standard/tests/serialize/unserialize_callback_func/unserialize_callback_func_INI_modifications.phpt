--TEST--
unserialize_callback_func INI setting set multiple times during runtime
--FILE--
<?php

function unserialize_cb_original($name)
{
    ini_set('unserialize_callback_func', 'unserialize_cb_changed');
}

function unserialize_cb_changed($name)
{
}

ini_set('unserialize_callback_func', 'unserialize_cb_original');

unserialize('O:3:"FOO":0:{}');

var_dump(ini_get('unserialize_callback_func'));

ini_restore('unserialize_callback_func');

var_dump(ini_get('unserialize_callback_func'));

?>
--EXPECTF--
Warning: unserialize(): Function unserialize_cb_original() hasn't defined the class it was called for in %s on line %d
string(22) "unserialize_cb_changed"
string(0) ""
