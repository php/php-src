--TEST--
Bug #22836 (returning references to NULL)
--FILE--
<?php
function &f()
{
    $x = "foo";
    var_dump($x);
    print "'$x'\n";
    return ($a);
}
for ($i = 0; $i < 8; $i++) {
    $h =& f();
}
?>
--EXPECT--
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
string(3) "foo"
'foo'
