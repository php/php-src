--TEST--
Test parse_str() function : test with badly formed strings
--XFAIL--
Bug 48658: Test currently fails as parse_str() api insists that the optional reult array 
is pre-defined. php.net bugzilla 
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