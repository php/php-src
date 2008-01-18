--TEST--
Bug #42548 PROCEDURE xxx can't return a result set in the given context (works in 5.2.3!!)
--SKIPIF--
<?php if (!extension_loaded("mysqli")) print "skip"; ?>
--FILE--
<?php
include "connect.inc";

$mysqli = mysqli_init();
$mysqli->real_connect($host, $user, $passwd, 'test');
if (mysqli_connect_errno()) {
  printf("Connect failed: %s\n", mysqli_connect_error());
  exit();
}

$mysqli->query("DROP PROCEDURE IF EXISTS p1") or die($mysqli->error);
$mysqli->query("CREATE PROCEDURE p1() BEGIN SELECT 23; SELECT 42; END") or die($mysqli->error);

if ($mysqli->multi_query("CALL p1();"))
{	
  do	
  {	
    if ($objResult = $mysqli->store_result()) {
      while ($row = $objResult->fetch_assoc()) {
        print_r($row);
      }
      $objResult->close();
      if ($mysqli->more_results()) {
        print "----- next result -----------\n";
      }
    } else {
      print "no results found";
    }
  } while ($mysqli->more_results() && $mysqli->next_result());
} else {
  print $mysqli->error;
}

$mysqli->query("DROP PROCEDURE p1") or die($mysqli->error);
$mysqli->close();
?>
--EXPECT--	
Array
(
    [23] => 23
)
----- next result -----------
Array
(
    [42] => 42
)
----- next result -----------
no results found
