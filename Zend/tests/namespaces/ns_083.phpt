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
Fatal error: Namespace declaration statement has to be the very first statement or after any declare call in the script in %sns_083.php on line %d
