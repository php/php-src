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
array(0) {
}
string(9) "a=1&b=ZYX"
