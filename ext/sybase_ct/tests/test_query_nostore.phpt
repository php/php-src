--TEST--
Sybase-CT query without storing
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

  // Create test table and insert some data
  var_dump(sybase_query('
    create table #test (
      id numeric(10, 0) primary key not null,
      caption varchar(255) not null,
      author varchar(50) not null,
      link varchar(255) null,
      lastchange datetime default getdate() null
    )
  ', $db));
  var_dump(sybase_query('insert into #test (
      id, caption, author
    ) values (
      1, "Hello", "timm"
    )
  ', $db));
  var_dump(sybase_query('insert into #test (
      id, caption, author, link
    ) values (
      2, "World", "thekid", "http://thekid.de/"
    )
  ', $db));
  var_dump(sybase_query('insert into #test (
      id, caption, author
    ) values (
      3, "PHP", "friebe"
    )
  ', $db));

  // Fetch data
  $q= sybase_unbuffered_query('select * from #test order by id', $db, FALSE);
  var_dump($q);
  while ($row= sybase_fetch_assoc($q)) {
    var_dump($row);
  }
  
  // Clean up and close connection
  var_dump(sybase_query('drop table #test'));
  sybase_close($db);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
resource(%d) of type (sybase-ct result)
array(5) {
  ["id"]=>
  int(1)
  ["caption"]=>
  string(5) "Hello"
  ["author"]=>
  string(4) "timm"
  ["link"]=>
  NULL
  ["lastchange"]=>
  string(%d) "%s"
}
array(5) {
  ["id"]=>
  int(2)
  ["caption"]=>
  string(5) "World"
  ["author"]=>
  string(6) "thekid"
  ["link"]=>
  string(17) "http://thekid.de/"
  ["lastchange"]=>
  string(%d) "%s"
}
array(5) {
  ["id"]=>
  int(3)
  ["caption"]=>
  string(3) "PHP"
  ["author"]=>
  string(6) "friebe"
  ["link"]=>
  NULL
  ["lastchange"]=>
  string(%d) "%s"
}
bool(true)
