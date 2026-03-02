--TEST--
Union and intersection type leaks
--FILE--
<?php
eval('abstract class y {function y(): (y&yy)|t {}}');
?>
DONE
--EXPECTF--
DONE
