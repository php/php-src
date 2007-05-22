--TEST--
Sybase-CT bug #28354 (sybase_free_result crash)
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
  sybase_set_message_handler('sybase_msg_handler');
  error_reporting(error_reporting() & !E_NOTICE);       // Suppress notices

  // Check if stored procedure already exists
  $sp_name= 'phpt_bug28354';
  var_dump(sybase_select_db(TEMPDB, $db));
  if (!sybase_select_single($db, 'select object_id("'.$sp_name.'")')) {
    echo "Stored procedure {$sp_name} not found, creating\n";
    var_dump(sybase_query('
      create proc '.$sp_name.' as begin
        select @@version
      end
    '));
  } else {
    echo "Stored procedure {$sp_name} found, using\n";
    var_dump(TRUE);
  }
  
  $r= sybase_query('exec '.$sp_name, $db);
  $i= 0;
  while ($row= sybase_fetch_row($r)) {
    var_dump($i++, $row[0]);
  }
  sybase_free_result($r);
  sybase_close($db);
?>
--EXPECTF--
bool(true)
Stored procedure %s
bool(true)
int(0)
string(%d) "%s"
