--TEST--
Bug #71298: MB_CASE_TITLE misbehaves with curled apostrophe/quote (HTML &rsquo;)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
echo mb_convert_case("People's issues versus people’s issues", MB_CASE_TITLE);
?>
--EXPECT--
People's Issues Versus People’s Issues
