--TEST--
Bug #25758 (var_export does not escape ' & \ inside array keys)
--FILE--
<?php
    $a = array ("quote'" => array("quote'"));
    echo var_export($a, true);
?>
--EXPECT--
array (
  'quote\'' => 
  array (
    0 => 'quote\'',
  ),
)
