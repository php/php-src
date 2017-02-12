--TEST--
Bug #65600 (SplFileObject->next() not move next without current())
--FILE--
<?php
$f = new SplFileObject('php://temp', 'w+');
$f->fwrite("line 1\nline 2\nline 3");

$f->rewind();
var_dump($f->key());
var_dump($f->current());

$f->rewind();
$f->next();
$f->next();
$f->next();
var_dump($f->key());
var_dump($f->current());
?>
--EXPECT--
int(0)
string(7) "line 1
"
int(0)
string(7) "line 1
"
