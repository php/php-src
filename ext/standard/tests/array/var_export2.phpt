--TEST--
var_export() and empty array keys
--FILE--
<?php
$a = array ("\0" => 'null', "" => 'empty', "0" => 'nul');
var_export($a);
?>
--EXPECT--
array (
  '' . "\0" . '' => 'null',
  '' => 'empty',
  0 => 'nul',
)
