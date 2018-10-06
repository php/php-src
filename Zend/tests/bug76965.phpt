--TEST--
Bug #76965 (INI_SCANNER_RAW doesn't strip trailing whitespace)
--FILE--
<?php
// the trailing whitespace is intentional
$ini = <<<END
1="foo" 
2="bar" ; comment 
3= baz 
END;

var_dump(parse_ini_string($ini, false, INI_SCANNER_RAW));
?>
===DONE===
--EXPECT--
array(3) {
  [1]=>
  string(3) "foo"
  [2]=>
  string(3) "bar"
  [3]=>
  string(3) "baz"
}
===DONE===
