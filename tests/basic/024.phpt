--TEST--
Test HTTP_RAW_POST_DATA creation
--INI--
always_populate_raw_post_data=1
max_input_vars=1000
--POST--
a=ABC&y=XYZ&c[]=1&c[]=2&c[a]=3
--FILE--
<?php
var_dump($_POST, $HTTP_RAW_POST_DATA);
?>
--EXPECT--
Deprecated: Automatically populating $HTTP_RAW_POST_DATA is deprecated and will be removed in a future version. To avoid this warning set 'always_populate_raw_post_data' to '-1' in php.ini and use the php://input stream instead. in Unknown on line 0

Warning: Cannot modify header information - headers already sent in Unknown on line 0
array(3) {
  ["a"]=>
  string(3) "ABC"
  ["y"]=>
  string(3) "XYZ"
  ["c"]=>
  array(3) {
    [0]=>
    string(1) "1"
    [1]=>
    string(1) "2"
    ["a"]=>
    string(1) "3"
  }
}
string(30) "a=ABC&y=XYZ&c[]=1&c[]=2&c[a]=3"
