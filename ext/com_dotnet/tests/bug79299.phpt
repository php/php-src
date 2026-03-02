--TEST--
Bug #79299 (com_print_typeinfo prints duplicate variables)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$dict = new COM("Scripting.Dictionary");
ob_start();
com_print_typeinfo($dict);
$typeinfo = ob_get_clean();
preg_match_all('/\/\* DISPID=9 \*\//', $typeinfo, $matches);
var_dump($matches[0]);
?>
--EXPECT--
array(1) {
  [0]=>
  string(14) "/* DISPID=9 */"
}