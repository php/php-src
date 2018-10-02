--TEST--
Bug #74371: strip_tags altering attributes
--FILE--
<?php

echo strip_tags('<img src="example.jpg" alt=":> :<">', '<img>');

?>
--EXPECT--
<img src="example.jpg" alt=":> :<">
