--TEST--
SPL: ArrayObject::exchangeArray() with various object arguments
--FILE--
<?php
echo "--> exchangeArray(array):\n";
$ao = new ArrayObject();
$ao->exchangeArray(array('key'=>'original'));
var_dump($ao['key']);
var_dump($ao);

echo "\n--> exchangeArray(normal object):\n";
$obj = new stdClass;
$obj->key = 'normal object prop';
$ao->exchangeArray($obj);
var_dump($ao['key']);
var_dump($ao);

echo "\n--> exchangeArray(ArrayObject):\n";
$obj = new ArrayObject(array('key'=>'ArrayObject element'));
$ao->exchangeArray($obj);
var_dump($ao['key']);
var_dump($ao);

echo "\n--> exchangeArray(ArrayIterator):\n";
$obj = new ArrayIterator(array('key'=>'ArrayIterator element'));
$ao->exchangeArray($obj);
var_dump($ao['key']);
var_dump($ao);

echo "\n--> exchangeArray(nested ArrayObject):\n";
$obj = new ArrayObject(new ArrayObject(array('key'=>'nested ArrayObject element')));
$ao->exchangeArray($obj);
var_dump($ao['key']);
var_dump($ao);
?>
--EXPECTF--
--> exchangeArray(array):
unicode(8) "original"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  array(1) {
    [u"key"]=>
    unicode(8) "original"
  }
}

--> exchangeArray(normal object):
unicode(18) "normal object prop"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  object(stdClass)#%d (1) {
    [u"key"]=>
    unicode(18) "normal object prop"
  }
}

--> exchangeArray(ArrayObject):
unicode(19) "ArrayObject element"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  object(ArrayObject)#%d (1) {
    [u"storage":u"ArrayObject":private]=>
    array(1) {
      [u"key"]=>
      unicode(19) "ArrayObject element"
    }
  }
}

--> exchangeArray(ArrayIterator):
unicode(21) "ArrayIterator element"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  object(ArrayIterator)#%d (1) {
    [u"storage":u"ArrayIterator":private]=>
    array(1) {
      [u"key"]=>
      unicode(21) "ArrayIterator element"
    }
  }
}

--> exchangeArray(nested ArrayObject):
unicode(26) "nested ArrayObject element"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  object(ArrayObject)#%d (1) {
    [u"storage":u"ArrayObject":private]=>
    object(ArrayObject)#%d (1) {
      [u"storage":u"ArrayObject":private]=>
      array(1) {
        [u"key"]=>
        unicode(26) "nested ArrayObject element"
      }
    }
  }
}