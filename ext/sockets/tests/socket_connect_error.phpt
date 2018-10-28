--TEST--
Test error cases when creating a socket
--CREDITS--
Russell Flynn <russ@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--INI--
error_reporting=E_ALL
display_errors=1
--SKIPIF--
<?php
  if (!extension_loaded('sockets')) {
  echo 'skip sockets extension not available.';
}
?>
--FILE--
<?php
  // Test with no arguments
  $server = socket_create();

  // Test with less arguments than required
  $server = socket_create(SOCK_STREAM, getprotobyname('tcp'));

  // Test with non integer parameters
  $server = socket_create(array(), 1, 1);

?>
--EXPECTF--
Warning: socket_create() expects exactly 3 parameters, 0 given in %s on line %d

Warning: socket_create() expects exactly 3 parameters, 2 given in %s on line %d

Warning: socket_create() expects parameter 1 to be int, array given in %s on line %d
