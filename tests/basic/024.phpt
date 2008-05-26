--TEST--
Test HTTP_RAW_POST_DATA creation
--INI--
magic_quotes_gpc=0
always_populate_raw_post_data=1
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
a=ABC&y=XYZ&c[]=1&c[]=2&c[a]=3
--FILE--
<?php
var_dump($_POST, $HTTP_RAW_POST_DATA);
?>
--EXPECT--
array(3) {
  [u"a"]=>
  unicode(3) "ABC"
  [u"y"]=>
  unicode(3) "XYZ"
  [u"c"]=>
  array(3) {
    [0]=>
    unicode(1) "1"
    [1]=>
    unicode(1) "2"
    [u"a"]=>
    unicode(1) "3"
  }
}
unicode(30) "a=ABC&y=XYZ&c[]=1&c[]=2&c[a]=3"
