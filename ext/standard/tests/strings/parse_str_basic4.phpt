--TEST--
Test parse_str() function : test with badly formed strings
--FILE--
<?php
/* Prototype  : void parse_str  ( string $str  [, array &$arr  ] )
 * Description: Parses the string into variables
 * Source code: ext/standard/string.c
*/

echo "\nTest string with badly formed strings\n";
$str = "arr[1=sid&arr[4][2=fred";
var_dump(parse_str($str, $res));
var_dump($res);

$str = "arr1]=sid&arr[4]2]=fred";
var_dump(parse_str($str, $res));
var_dump($res);

$str = "arr[one=sid&arr[4][two=fred";
var_dump(parse_str($str, $res));
var_dump($res);

echo "\nTest string with badly formed % numbers\n";
$str = "first=%41&second=%a&third=%b";
var_dump(parse_str($str));
var_dump($first, $second, $third);

echo "\nTest string with non-binary safe name\n";
$str = "arr.test[1]=sid&arr test[4][two]=fred";
var_dump(parse_str($str, $res));
var_dump($res);
?>
===DONE===
--EXPECTF--
Test string with badly formed strings
NULL
array(2) {
  [u"arr_1"]=>
  unicode(3) "sid"
  [u"arr"]=>
  array(1) {
    [4]=>
    unicode(4) "fred"
  }
}
NULL
array(2) {
  [u"arr1]"]=>
  unicode(3) "sid"
  [u"arr"]=>
  array(1) {
    [4]=>
    unicode(4) "fred"
  }
}
NULL
array(2) {
  [u"arr_one"]=>
  unicode(3) "sid"
  [u"arr"]=>
  array(1) {
    [4]=>
    unicode(4) "fred"
  }
}

Test string with badly formed % numbers
NULL
unicode(1) "A"
unicode(2) "%a"
unicode(2) "%b"

Test string with non-binary safe name
NULL
array(1) {
  [u"arr_test"]=>
  array(2) {
    [1]=>
    unicode(3) "sid"
    [4]=>
    array(1) {
      [u"two"]=>
      unicode(4) "fred"
    }
  }
}
===DONE===