--TEST--
Specifying non-default syntax in mb_ereg_search()
--FILE--
<?php

mb_ereg_search_init("a");
var_dump(mb_ereg_search("a\\{1,2\\}", "b"));

?>
--EXPECT--
bool(true)
