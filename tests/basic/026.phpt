--TEST--
Registration of HTTP_RAW_POST_DATA due to unknown content-type
--INI--
always_populate_raw_post_data=0
--POST_RAW--
Content-Type: unknown/type
a=1&b=ZYX
--FILE--
<?php
var_dump($_POST, $HTTP_RAW_POST_DATA);
?>
--EXPECT--
Deprecated: Automatically populating $HTTP_RAW_POST_DATA is deprecated and will be removed in a future version. To avoid this warning set 'always_populate_raw_post_data' to '-1' in php.ini and use the php://input stream instead. in Unknown on line 0

Warning: Cannot modify header information - headers already sent in Unknown on line 0
array(0) {
}
string(9) "a=1&b=ZYX"
