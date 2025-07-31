--TEST--
eval() constant resolution
--FILE--
<?php
namespace foo {
define('foo\true', 'test');
echo "In eval\n";
eval('namespace foo { var_dump(true); var_dump(TrUe); var_dump(namespace\true); var_dump(\true); }');
echo "Outside eval\n";
var_dump(true); var_dump(TrUe); var_dump(namespace\true); var_dump(\true);
}
?>
--EXPECT--
In eval
bool(true)
bool(true)
string(4) "test"
bool(true)
Outside eval
bool(true)
bool(true)
string(4) "test"
bool(true)