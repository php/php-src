--TEST--
Bug #76643 (Segmentation fault when using `output_add_rewrite_var`)
--FILE--
<?php
$_SERVER = 'foo';
output_add_rewrite_var('bar', 'baz');
?>
<form action="http://example.com/"></form>
===DONE===
--EXPECT--
<form action="http://example.com/"></form>
===DONE===
