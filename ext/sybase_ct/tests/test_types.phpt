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

  $db= sybase_connect_ex();
  var_dump(sybase_select_ex($db, 'select 
    1 as "integer", 
    -2147483647 as "integer_min",
    -2147483648 as "integer_min_exceed",
    2147483647 as "integer_max",
    2147483648 as "integer_max_exceed",
    1.0  as "float", 
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
    -2147483647 as "integer_min",
    -2147483648 as "integer_min_exceed",
    2147483647 as "integer_max",
    2147483648 as "integer_max_exceed",
    1.0  as "float", 
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
    int(-2147483647)
    ["integer_min_exceed"]=>
    float(-2147483648)
    ["integer_max"]=>
    int(2147483647)
    ["integer_max_exceed"]=>
    float(2147483648)
    ["float"]=>
    float(1)
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
