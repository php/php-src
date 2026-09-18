--TEST--
Test func ZPP specifier (strict_mode)
--EXTENSIONS--
zend_test
--FILE--
<?php

declare(strict_types=1);

$types = require 'types.inc';
$types['strlen'] = 'strlen';

$functions = [
    'zend_func',
    'zend_func_or_null',
    'zend_func_no_trampoline_free',
    'zend_func_no_trampoline_free_or_null',
    'zend_func_slow_zpp',
    'zend_func_or_null_slow_zpp',
];

foreach ($functions as $i => $function) {
    echo "Using $function:\n\n";

    foreach ($types as $name => $type) {
        printf("  %-16s ", "$name:");

        try {
            $result = $function($type);

            $result === []
                ? printf("array(0) {}\n")
                : var_dump($result);

        } catch (Throwable $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
    }

    if ($i !== array_key_last($functions)) {
        echo "\n";
    }
}

?>
--EXPECT--
Using zend_func:

  null:            TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  false:           TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  true:            TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  42:              TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  73.5:            TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  'string':        TypeError: zend_func(): Argument #1 ($param) must be a valid callback, function "string" not found or invalid function name
  '15':            TypeError: zend_func(): Argument #1 ($param) must be a valid callback, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func(): Argument #1 ($param) must be a valid callback, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func(): Argument #1 ($param) must be a valid callback, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func(): Argument #1 ($param) must be a valid callback, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func(): Argument #1 ($param) must be a valid callback, array callback must have exactly two members
  new stdClass():  TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  new S():         TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  STDOUT:          TypeError: zend_func(): Argument #1 ($param) must be a valid callback, no array or string given
  strlen:          string(6) "strlen"

Using zend_func_or_null:

  null:            NULL
  false:           TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  true:            TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  42:              TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  73.5:            TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  'string':        TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, function "string" not found or invalid function name
  '15':            TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, array callback must have exactly two members
  new stdClass():  TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  new S():         TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  STDOUT:          TypeError: zend_func_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  strlen:          string(6) "strlen"

Using zend_func_no_trampoline_free:

  null:            TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  false:           TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  true:            TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  42:              TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  73.5:            TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  'string':        TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, function "string" not found or invalid function name
  '15':            TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, array callback must have exactly two members
  new stdClass():  TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  new S():         TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  STDOUT:          TypeError: zend_func_no_trampoline_free(): Argument #1 ($param) must be a valid callback, no array or string given
  strlen:          string(6) "strlen"

Using zend_func_no_trampoline_free_or_null:

  null:            NULL
  false:           TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  true:            TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  42:              TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  73.5:            TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  'string':        TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, function "string" not found or invalid function name
  '15':            TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, array callback must have exactly two members
  new stdClass():  TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  new S():         TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  STDOUT:          TypeError: zend_func_no_trampoline_free_or_null(): Argument #1 ($param) must be a valid callback or null, no array or string given
  strlen:          string(6) "strlen"

Using zend_func_slow_zpp:

  null:            TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  false:           TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  true:            TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  42:              TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  73.5:            TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  'string':        TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, function "string" not found or invalid function name
  '15':            TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, array callback must have exactly two members
  new stdClass():  TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  new S():         TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  STDOUT:          TypeError: zend_func_slow_zpp(): Argument #1 ($param) must be a valid callback, no array or string given
  strlen:          string(6) "strlen"

Using zend_func_or_null_slow_zpp:

  null:            NULL
  false:           TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  true:            TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  42:              TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  73.5:            TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  'string':        TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, function "string" not found or invalid function name
  '15':            TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, function "15" not found or invalid function name
  '56.7':          TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, function "56.7" not found or invalid function name
  'stdClass':      TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, function "stdClass" not found or invalid function name
  anon class name: TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, function "class@anonymous" not found or invalid function name
  []:              TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, array callback must have exactly two members
  new stdClass():  TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  new S():         TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  STDOUT:          TypeError: zend_func_or_null_slow_zpp(): Argument #1 ($param) must be a valid callback or null, no array or string given
  strlen:          string(6) "strlen"
