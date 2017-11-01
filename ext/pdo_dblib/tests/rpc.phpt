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
$prms = [];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC));
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC_SKIP_RESULTS));
$st->bindValue('stmt', 'set @b = @a');
$st->bindValue('params', '@a varchar(10), @b varchar(10) out');
$st->bindValue('a', 'test!');
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->bindParam('RETVAL', $prms['r'], PDO::PARAM_INPUT_OUTPUT);
$st->execute();
var_dump($prms);

/* error */
$st = $db->prepare('sp_doesnt_exist', [PDO::DBLIB_ATTR_RPC => 1]);
try {
  $st->execute();
} catch (PDOException $e) {
  $st = null;
  if (strpos($e->getMessage(), "not find stored procedure 'sp_doesnt_exist'") > -1) echo "ok\n";
}

/* numbered params / mixed / unsorted */
$prms = [];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
$st->bindValue('a', 'numbered');
$st->bindValue(2, '@a varchar(10), @b varchar(10) out');
$st->bindParam('RETVAL', $prms['r'], PDO::PARAM_INPUT_OUTPUT);
$st->bindValue(1, 'set @b = @a');
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->execute();
var_dump($prms);

/* re-bind */
$st->bindValue('a', 're-bound');
$st->execute();
var_dump($prms['b']);

/* types */
$prms = ['c' => 1, 'd' => 1.2, 'e' => 'test'];
for ($i=0; $i<12; $i++) $prms['e'] .= $prms['e'];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
$st->bindValue('stmt', 'set @b = @a; select b=@b, c=@c, d=@d, e=len(@e)');
$st->bindValue('params', '@a varchar(10), @c float=null, @d float=null, @e varchar(max)=null, @b varchar(10) out');
$st->bindValue('a', 'select');
$st->bindParam('b', $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->bindParam('c', $prms['c'], PDO::PARAM_INT);
$st->bindParam('d', $prms['d'], PDO::PARAM_INT);
$st->bindParam('e', $prms['e']);
$st->execute();
var_dump($st->fetch(PDO::FETCH_ASSOC));
var_dump($prms['b']);

/* skip results */
$prms = ['a' => 'selectskip'];
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1, PDO::DBLIB_ATTR_RPC_SKIP_RESULTS => 1]);
var_dump($st->getAttribute(PDO::DBLIB_ATTR_RPC_SKIP_RESULTS));
$st->bindValue(1, 'set @b = @a; select @a, @b');
$st->bindValue(2, '@a varchar(10), @b varchar(10) out');
$st->bindParam(3, $prms['a']);
$st->bindParam(4, $prms['b'], PDO::PARAM_INPUT_OUTPUT);
$st->bindParam('RETVAL', $prms['r'], PDO::PARAM_INPUT_OUTPUT);
$st->execute();
var_dump($prms);

/* redo */
$prms['a'] = 'redo';
$st->execute();
var_dump($prms['b']);

/* bindValue shortcut */
$st = $db->prepare('sp_executesql', [PDO::DBLIB_ATTR_RPC => 1]);
$st->execute([
  'select a=@a',
  '@a varchar(10)',
  "sim'ple"
]);
var_dump($st->fetch(PDO::FETCH_ASSOC));
?>
--EXPECT--
bool(false)
array(1) {
  ["test"]=>
  string(1) "1"
}
bool(true)
bool(false)
array(2) {
  ["b"]=>
  &string(5) "test!"
  ["r"]=>
  &int(0)
}
ok
array(2) {
  ["r"]=>
  &int(0)
  ["b"]=>
  &string(8) "numbered"
}
string(8) "re-bound"
array(4) {
  ["b"]=>
  string(6) "select"
  ["c"]=>
  float(1)
  ["d"]=>
  float(1.2)
  ["e"]=>
  int(16384)
}
string(6) "select"
bool(true)
array(3) {
  ["a"]=>
  &string(10) "selectskip"
  ["b"]=>
  &string(10) "selectskip"
  ["r"]=>
  &int(0)
}
string(4) "redo"
array(1) {
  ["a"]=>
  string(7) "sim'ple"
}
