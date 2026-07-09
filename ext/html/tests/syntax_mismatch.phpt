--TEST--
Markup syntax: a mismatched closing tag is a compile error
--EXTENSIONS--
html
--FILE--
<?php
$x = <div>oops</span>;
?>
--EXPECTF--
Fatal error: Mismatched markup closing tag: expected </div>, found </span> in %s on line %d
