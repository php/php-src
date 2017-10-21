--TEST--
PDO_DBLIB: RPC Support
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';
$db->query('set language english');

/* non RPC */
$st = $db->prepare('select :test as test');
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC));
$st->execute([':test' => 1]);
var_dump($st->fetch(PDO::FETCH_ASSOC));

/* RPC */
$prms = ['a' => 'test', 'b' => null];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC));
$st->bindValue('stmt', 'set @b = @a');
$st->bindValue('params', '@a varchar(10), @b varchar(10) out');
$st->bindParam('a', $prms['a']);
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->bindParam('RETVAL', $prms['r'], PDO::PARAM_INPUT_OUTPUT);
$st->execute();
var_dump($prms);

/* types */
$prms = ['a' => 'sel', 'b' => null, 'c' => 1, 'd' => 1.2, 'e' => 'test'];
for ($i=0; $i<12; $i++) $prms['e'] .= $prms['e'];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC_SKIP_RESULTS));
$st->bindValue('stmt', 'set @b = @a; select b=@b, c=@c, d=@d, e=len(@e)');
$st->bindValue('params', '@a varchar(10), @c float=null, @d float=null, @e varchar(max)=null, @b varchar(10) out');
$st->bindParam('a', $prms['a']);
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->bindParam('c', $prms['c'], PDO::PARAM_INT);
$st->bindParam('d', $prms['d'], PDO::PARAM_INT);
$st->bindParam('e', $prms['e']);
$st->execute();
var_dump($st->fetch(PDO::FETCH_ASSOC));
var_dump($prms['b']);

/* fetch skip */
$prms = ['a' => 'selskip', 'b' => null];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1, PDO::DBLIB_ATTR_RPC_SKIP_RESULTS => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC_SKIP_RESULTS));
$st->bindValue('stmt', 'set @b = @a; select @a');
$st->bindValue('params', '@a varchar(10), @b varchar(10) out');
$st->bindParam('a', $prms['a']);
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->execute();
var_dump($prms['b']);
?>
--EXPECT--
bool(false)
array(1) {
  ["test"]=>
  string(1) "1"
}
bool(true)
array(3) {
  ["a"]=>
  &string(4) "test"
  ["b"]=>
  &string(4) "test"
  ["r"]=>
  &int(0)
}
bool(false)
array(4) {
  ["b"]=>
  string(3) "sel"
  ["c"]=>
  float(1)
  ["d"]=>
  float(1.2)
  ["e"]=>
  int(16384)
}
string(3) "sel"
bool(true)
string(7) "selskip"
