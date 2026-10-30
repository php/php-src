--TEST--
GH-17037 (UAF in user filter when adding existing filter name due to incorrect error handling)
--FILE--
<?php
var_dump(stream_filter_register('string.toupper', 'filter_string_toupper'));
?>
--EXPECT--
bool(false)
