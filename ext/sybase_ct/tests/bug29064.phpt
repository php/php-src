--TEST--
Sybase-CT bug #29064 (Exact numeric/decimal/money datatypes lose precision)
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
  
  // Create a temporary table and fill it with test values
  var_dump(sybase_query('
    create table #test (
      test_decimal      decimal(38, 6)  null,
      test_numeric      numeric(38, 12) null,
      test_money        money           null,
      test_bigint       decimal(38, 0)  null,
      test_int          int             null,
      test_smallmoney   smallmoney      null,
      test_smallint     smallint        null,
      test_tinyint      tinyint         null,
      test_real         float           null,
      test_double       float           null
    )
  ', $db));
  var_dump(sybase_query('
    insert into #test (
      test_decimal, 
      test_numeric, 
      test_money, 
      test_bigint, 
      test_int,
      test_smallmoney, 
      test_smallint, 
      test_tinyint, 
      test_real, 
      test_double
    ) values (
      12345678901234567890123456789012.123456,
      12345678901234567890123456.123456789012,
      123456789012345.1234,
      12345678901234567890123456789012345678,
      1234567890,
      123456.1234,
      12345,
      123,
      123456789.12345679,
      123456789.12345679
    )
  ', $db));
  var_dump(sybase_query('
    insert into #test (
      test_decimal, 
      test_numeric, 
      test_money, 
      test_bigint, 
      test_int,
      test_smallmoney, 
      test_smallint, 
      test_tinyint, 
      test_real, 
      test_double
    ) values (
      -12345678901234567890123456789012.123456,
      -12345678901234567890123456.123456789012,
      -123456789012345.1234,
      -12345678901234567890123456789012345678,
      -1234567890,
      -123456.1234,
      -12345,
      255,
      -123456789.12345679,
      -123456789.12345679
    )
  ', $db));

  // Select the data  
  var_dump(sybase_select_ex($db, 'select * from #test'));
  
  // Clean up and close connection
  var_dump(sybase_query('drop table #test', $db));
  sybase_close($db);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
>>> Query: select * from #test
<<< Return: resource
array(2) {
  [0]=>
  array(10) {
    ["test_decimal"]=>
    string(39) "12345678901234567890123456789012.123456"
    ["test_numeric"]=>
    string(39) "12345678901234567890123456.123456789012"
    ["test_money"]=>
    string(18) "123456789012345.12"
    ["test_bigint"]=>
    string(38) "12345678901234567890123456789012345678"
    ["test_int"]=>
    int(1234567890)
    ["test_smallmoney"]=>
    float(123456.12)
    ["test_smallint"]=>
    int(12345)
    ["test_tinyint"]=>
    int(123)
    ["test_real"]=>
    string(18) "123456789.12345679"
    ["test_double"]=>
    string(18) "123456789.12345679"
  }
  [1]=>
  array(10) {
    ["test_decimal"]=>
    string(40) "-12345678901234567890123456789012.123456"
    ["test_numeric"]=>
    string(40) "-12345678901234567890123456.123456789012"
    ["test_money"]=>
    string(19) "-123456789012345.12"
    ["test_bigint"]=>
    string(39) "-12345678901234567890123456789012345678"
    ["test_int"]=>
    int(-1234567890)
    ["test_smallmoney"]=>
    float(-123456.12)
    ["test_smallint"]=>
    int(-12345)
    ["test_tinyint"]=>
    int(255)
    ["test_real"]=>
    string(19) "-123456789.12345679"
    ["test_double"]=>
    string(19) "-123456789.12345679"
  }
}
bool(true)
