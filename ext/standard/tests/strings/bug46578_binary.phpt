--TEST--
Bug #46578 (strip_tags() does not honor end-of-comment when it encounters a single quote), binary variant
--FILE--
<?php

var_dump(strip_tags(b'<!-- testing I\'ve been to mars -->foobar'));

var_dump(strip_tags(b'<a alt="foobar">foo<!-- foo! --></a>bar'));

var_dump(strip_tags(b'<a alt="foobar"/>foo<?= foo! /* <!-- "cool" --> */ ?>bar'));

var_dump(strip_tags(b'< ax'));

var_dump(strip_tags(b'<! a>'));

var_dump(strip_tags(b'<? ax'));

?>
--EXPECTF--
string(6) "foobar"
string(6) "foobar"
string(6) "foobar"
string(4) "< ax"
string(0) ""
string(0) ""
