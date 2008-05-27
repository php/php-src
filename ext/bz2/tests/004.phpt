--TEST--
bzread() tests with invalid files
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php

$fd = bzopen(dirname(__FILE__)."/004_1.txt.bz2","r");
var_dump(bzerror($fd));
var_dump(bzerrstr($fd));
var_dump(bzerrno($fd));

$fd2 = bzopen(dirname(__FILE__)."/004_2.txt.bz2","r");
var_dump(bzerror($fd2));
var_dump(bzerrstr($fd2));
var_dump(bzerrno($fd2));

var_dump(bzread($fd, 10));
var_dump(bzerror($fd));
var_dump(bzerrstr($fd));
var_dump(bzerrno($fd));

var_dump(bzread($fd2, 10));
var_dump(bzerror($fd2));
var_dump(bzerrstr($fd2));
var_dump(bzerrno($fd2));

var_dump(bzread($fd));
var_dump(bzerror($fd));
var_dump(bzerrstr($fd));
var_dump(bzerrno($fd));

var_dump(bzread($fd2));
var_dump(bzerror($fd2));
var_dump(bzerrstr($fd2));
var_dump(bzerrno($fd2));

bzclose($fd2);
var_dump(bzread($fd2));
var_dump(bzerror($fd2));
var_dump(bzerrstr($fd2));
var_dump(bzerrno($fd2));

echo "Done\n";
?>
--EXPECTF--
array(2) {
  [u"errno"]=>
  int(0)
  [u"errstr"]=>
  unicode(2) "OK"
}
unicode(2) "OK"
int(0)
array(2) {
  [u"errno"]=>
  int(0)
  [u"errstr"]=>
  unicode(2) "OK"
}
unicode(2) "OK"
int(0)
string(0) ""
array(2) {
  [u"errno"]=>
  int(-5)
  [u"errstr"]=>
  unicode(16) "DATA_ERROR_MAGIC"
}
unicode(16) "DATA_ERROR_MAGIC"
int(-5)
string(0) ""
array(2) {
  [u"errno"]=>
  int(-4)
  [u"errstr"]=>
  unicode(10) "DATA_ERROR"
}
unicode(10) "DATA_ERROR"
int(-4)
string(0) ""
array(2) {
  [u"errno"]=>
  int(-5)
  [u"errstr"]=>
  unicode(16) "DATA_ERROR_MAGIC"
}
unicode(16) "DATA_ERROR_MAGIC"
int(-5)
string(0) ""
array(2) {
  [u"errno"]=>
  int(-4)
  [u"errstr"]=>
  unicode(10) "DATA_ERROR"
}
unicode(10) "DATA_ERROR"
int(-4)

Warning: bzread(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerror(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerrstr(): %d is not a valid stream resource in %s on line %d
bool(false)

Warning: bzerrno(): %d is not a valid stream resource in %s on line %d
bool(false)
Done
