--TEST--
Allow varargs in calls in constants
--INI--
error_reporting=E_ALL
--FILE--
<?php
const ARGS = [[100], [2]];
const RESULT = array_merge(...ARGS);
const RESULT_LINE = ARRAY_MERGE([600], ...[RESULT]);
var_export(RESULT_LINE);
echo "\n";
var_export(RESULT_LINE);
echo "\n";
--EXPECTF--
array (
  0 => 600,
  1 => 100,
  2 => 2,
)
array (
  0 => 600,
  1 => 100,
  2 => 2,
)