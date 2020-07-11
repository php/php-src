--TEST--
Bug #72885 flatfile: dba_fetch() fails to read replaced entry
--SKIPIF--
<?php
$handler = "flatfile";
require_once(__DIR__ .'/skipif.inc');
?>
--FILE--
<?php

require_once(__DIR__ .'/test.inc');

$db = dba_open($db_filename, 'c', 'flatfile');
dba_insert('foo', 'bar', $db);
var_dump(dba_replace('foo', 'baz', $db));
var_dump(dba_fetch('foo', $db));
dba_close($db);

?>
--CLEAN--
<?php
require_once(__DIR__ .'/clean.inc');
?>
--EXPECT--
bool(true)
string(3) "baz"
