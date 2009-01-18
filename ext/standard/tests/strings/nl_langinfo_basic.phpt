--TEST--
Test nl_langinfo() function : basic functionality 
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN'){  
  die('skip Not for Windows');
}
?>
--FILE--
<?php

/* Prototype  : string nl_langinfo  ( int $item  )
 * Description: Query language and locale information
 * Source code: ext/standard/string.c
*/

echo "*** Testing nl_langinfo() : basic functionality ***\n";

$original = setlocale(LC_ALL, 'C');

var_dump(nl_langinfo(ABDAY_2));
var_dump(nl_langinfo(DAY_4));
var_dump(nl_langinfo(ABMON_7));
var_dump(nl_langinfo(MON_4));
var_dump(nl_langinfo(RADIXCHAR));

setlocale(LC_ALL, $original); 
?>
===DONE===
--EXPECTF--
*** Testing nl_langinfo() : basic functionality ***

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d

Deprecated: nl_langinfo(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(3) "Mon"

Deprecated: nl_langinfo(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(9) "Wednesday"

Deprecated: nl_langinfo(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(3) "Jul"

Deprecated: nl_langinfo(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(5) "April"

Deprecated: nl_langinfo(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
string(1) "."

Deprecated: setlocale(): deprecated in Unicode mode, please use ICU locale functions in %s on line %d
===DONE===