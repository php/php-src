--TEST--
Sybase-CT select and types
--SKIPIF--
<?php require('skipif.inc'); ?>
--FILE--
<?php
/* This file is part of PHP test framework for ext/sybase_ct
 *
 * $Id$ 
 */

  require('test.inc');

  define('LONG_MAX', is_int(2147483648) ? 9223372036854775807 : 2147483647);
  define('LONG_MIN', -LONG_MAX - 1);

  $db= sybase_connect_ex();
  var_dump(sybase_select_ex($db, 'select 
    1 as "integer", 
    '.(LONG_MIN).' as "integer_min",
    '.(LONG_MIN - 1).' as "integer_min_exceed",
    '.(LONG_MAX).' as "integer_max",
    '.(LONG_MAX + 1).' as "integer_max_exceed",
    1.0  as "float",
    12345678901234567890123456789012.123456 as "large_float",
    $22.36 as "money",
    "Binford" as "string",
    convert(datetime, "2004-01-23") as "date",
    NULL as "null",
    convert(bit, 1) as "bit",
    convert(smalldatetime, "2004-01-23") as "smalldate",
    convert(char(10), "char") as "char10"
  '));
      
  sybase_close($db);
?>
--EXPECTF--
>>> Query: select 
    1 as "integer", 
    -%s as "integer_min",
    -%s as "integer_min_exceed",
    %s as "integer_max",
    %s as "integer_max_exceed",
    1.0  as "float",
    12345678901234567890123456789012.123456 as "large_float",
    $22.36 as "money",
    "Binford" as "string",
    convert(datetime, "2004-01-23") as "date",
    NULL as "null",
    convert(bit, 1) as "bit",
    convert(smalldatetime, "2004-01-23") as "smalldate",
    convert(char(10), "char") as "char10"
  
<<< Return: resource
array(1) {
  [0]=>
  array(%d) {
    ["integer"]=>
    int(1)
    ["integer_min"]=>
    int(-%s)
    ["integer_min_exceed"]=>
    float(-%s)
    ["integer_max"]=>
    int(%s)
    ["integer_max_exceed"]=>
    float(%s)
    ["float"]=>
    float(1)
    ["large_float"]=>
    string(39) "12345678901234567890123456789012.123456"
    ["money"]=>
    float(22.36)
    ["string"]=>
    string(7) "Binford"
    ["date"]=>
    string(19) "Jan 23 2004 12:00AM"
    ["null"]=>
    NULL
    ["bit"]=>
    int(1)
    ["smalldate"]=>
    string(19) "Jan 23 2004 12:00AM"
    ["char10"]=>
    string(10) "char      "
  }
}
