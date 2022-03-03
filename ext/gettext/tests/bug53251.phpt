--TEST--
Bug #53251 (bindtextdomain with null dir doesn't return old value)
--EXTENSIONS--
gettext
--SKIPIF--
<?php
if (getenv('SKIP_REPEAT')) die('skip gettext leaks global state across requests');
?>
--FILE--
<?php
var_dump(is_string(bindtextdomain('foo', null)));
$dir = bindtextdomain('foo', '.');
var_dump(bindtextdomain('foo', null) === $dir);

var_dump(bind_textdomain_codeset('foo', null));
var_dump(bind_textdomain_codeset('foo', 'UTF-8'));
var_dump(bind_textdomain_codeset('foo', null));

?>
--EXPECT--
bool(true)
bool(true)
bool(false)
string(5) "UTF-8"
string(5) "UTF-8"
