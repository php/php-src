--TEST--
Tests that the AST export of a static class includes the static modifier
--FILE--
<?php

assert(false && function () {
    static class C {}
});
?>
--EXPECTF--
%a
%wstatic class C {%w}
%a
