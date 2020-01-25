--TEST--
Allow varargs in calls in constants
--INI--
error_reporting=E_ALL
--FILE--
<?php
const ARGS = ['Hello, %s', 'World'];
const RESULT = sprintf(...ARGS);
const RESULT_LINE = sprintf("%s\n", ...[RESULT]);
echo RESULT_LINE;
echo RESULT_LINE;
--EXPECTF--
Hello, World
Hello, World
