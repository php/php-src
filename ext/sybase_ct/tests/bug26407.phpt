--TEST--
Sybase-CT bug #26407 (Result set fetching broken around transactions)
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

  // Create temporary table
  $table= 'phpt_bug26407';
  var_dump(sybase_query('create table #'.$table.' ( the_big_answer int )', $db));

  // I
  var_dump(sybase_select_ex($db, '
    begin transaction
      -- anything producing a result set here will fail;
      -- however, print or update statements will work
      select "foo" 
    commit
    -- anything afterwards will fail, too
  '));  

  // II
  var_dump(sybase_select_ex($db, '
    begin transaction
      -- no result returned...
      update #'.$table.' set the_big_answer=42
    commit
  '));
  
  // III
  var_dump(sybase_select_ex($db, '
    select "foo"
    begin transaction
      -- do anything, even return a result set
    commit
    select "bar"   
  '));

  sybase_close($db);
?>
--EXPECTF--
bool(true)
>>> Query: 
    begin transaction
      -- anything producing a result set here will fail;
      -- however, print or update statements will work
      select "foo" 
    commit
    -- anything afterwards will fail, too
  
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["computed"]=>
    string(3) "foo"
  }
}
>>> Query: 
    begin transaction
      -- no result returned...
      update #phpt_bug26407 set the_big_answer=42
    commit
  
<<< Return: boolean
bool(true)
>>> Query: 
    select "foo"
    begin transaction
      -- do anything, even return a result set
    commit
    select "bar"   
  

Notice: sybase_query(): Sybase:  Unexpected results, canceling current in %stest.inc on line %d
<<< Return: resource
array(1) {
  [0]=>
  array(1) {
    ["computed"]=>
    string(3) "foo"
  }
}
