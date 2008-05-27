--TEST--
Testing class extending to ArrayObject and serialize
--FILE--
<?php

class Name extends ArrayObject
{
    public $var = 'a';
    protected $bar = 'b';
    private $foo = 'c';
}

$a = new Name();
var_dump($a);
var_dump($a->var);

$a = unserialize(serialize($a));

var_dump($a);
var_dump($a->var);

class Bla extends ArrayObject
{
    public $var = 'aaa';
    protected $bar = 'bbb';
    private $foo = 'ccc';
}

$a = new Bla();
var_dump($a);
var_dump($a->var);

$a = unserialize(serialize($a));

var_dump($a);
var_dump($a->var);

?>
--EXPECT--
object(Name)#1 (4) {
  [u"var"]=>
  unicode(1) "a"
  [u"bar":protected]=>
  unicode(1) "b"
  [u"foo":u"Name":private]=>
  unicode(1) "c"
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
unicode(1) "a"
object(Name)#2 (4) {
  [u"var"]=>
  unicode(1) "a"
  [u"bar":protected]=>
  unicode(1) "b"
  [u"foo":u"Name":private]=>
  unicode(1) "c"
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
unicode(1) "a"
object(Bla)#1 (4) {
  [u"var"]=>
  unicode(3) "aaa"
  [u"bar":protected]=>
  unicode(3) "bbb"
  [u"foo":u"Bla":private]=>
  unicode(3) "ccc"
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
unicode(3) "aaa"
object(Bla)#2 (4) {
  [u"var"]=>
  unicode(3) "aaa"
  [u"bar":protected]=>
  unicode(3) "bbb"
  [u"foo":u"Bla":private]=>
  unicode(3) "ccc"
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
unicode(3) "aaa"
