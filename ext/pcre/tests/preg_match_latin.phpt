--TEST--
preg_match() single line match with latin input
--FILE--
<?php
/* Prototype  : int preg_match  ( string $pattern  , string $subject  [, array &$matches  [, int $flags  [, int $offset  ]]] )
 * Description: Perform a regular expression match
 * Source code: ext/pcre/php_pcre.c
 */

preg_match('/^[\w\p{Cyrillic}\s\-\']+$/u', 'latin', $test1);
preg_match('/^[\w\p{Cyrillic}\s\-\']+$/u', 'кириллица', $test2);
preg_match('/^[\w\s\-\']+$/u', 'latin', $test3);

var_dump([$test1, $test2, $test3]);
?>
===Done===
--EXPECT--
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(5) "latin"
  }
  [1]=>
  array(1) {
    [0]=>
    string(18) "кириллица"
  }
  [2]=>
  array(1) {
    [0]=>
    string(5) "latin"
  }
}
===Done===
