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
--EXPECTF--
string(3) "foo"
'foo'

Notice: Undefined variable:  a in %s on line %d

Fatal error: Cannot return NULL by reference in %s on line %d
