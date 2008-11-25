--TEST--
083: bracketed namespace with junk before the ns declaration
--FILE--
<?php
$a = 'oops';
echo $a;
namespace foo {
}
namespace ok {
echo "ok\n";
}
?>
--EXPECTF--
Fatal error: Namespace declaration statement has to be the very first statement in the script in %s on line %d
