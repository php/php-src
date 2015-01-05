--TEST--
Bug #46578 (strip_tags() does not honor end-of-comment when it encounters a single quote)
--FILE--
<?php

var_dump(strip_tags('<!-- testing I\'ve been to mars -->foobar'));

var_dump(strip_tags('<a alt="foobar">foo<!-- foo! --></a>bar'));

var_dump(strip_tags('<a alt="foobar"/>foo<?= foo! /* <!-- "cool" --> */ ?>bar'));

var_dump(strip_tags('< ax'));

var_dump(strip_tags('<! a>'));

var_dump(strip_tags('<? ax'));

?>
--EXPECTF--
%string|unicode%(6) "foobar"
%string|unicode%(6) "foobar"
%string|unicode%(6) "foobar"
%string|unicode%(4) "< ax"
%string|unicode%(0) ""
%string|unicode%(0) ""
