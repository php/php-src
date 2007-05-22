--TEST--
Sybase-CT unbuffered query
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
  var_dump($db);
  
  // Test #1: num_rows
  $q= sybase_unbuffered_query('select name from master..systypes', $db);
  var_dump($q);
  $i= 0;
  while ($r= sybase_fetch_assoc($q)) {
    $i++;
  }
  var_dump($i, sybase_num_rows($q), $i == sybase_num_rows($q), $q);
  sybase_free_result($q);
  var_dump($q);
  
  // Test #2: after sybase_free_result, this should be an invalid resource
  var_dump(FALSE == sybase_num_rows($q));
  
  // Test #3: Seeking
  $q= sybase_unbuffered_query('select name from master..systypes', $db);
  var_dump($q);
  $j= 0;
  sybase_data_seek($q, 4);
  while ($r= sybase_fetch_assoc($q)) {
    $j++;
  }
  var_dump($i, $j, $i - $j);
  
  sybase_close($db);
?>
--EXPECTF--
resource(%d) of type (sybase-ct link)
resource(%d) of type (sybase-ct result)
int(%d)
int(%d)
bool(true)
resource(%d) of type (sybase-ct result)
resource(%d) of type (Unknown)

Warning: sybase_num_rows(): %d is not a valid Sybase result resource in %stest_unbuffered_query.php on line %d
bool(true)
resource(%d) of type (sybase-ct result)
int(%d)
int(%d)
int(4)
