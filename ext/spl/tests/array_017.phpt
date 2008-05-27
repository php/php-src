--TEST--
SPL: ArrayObject::exchangeArray($this)
--FILE--
<?php

class ArrayIteratorEx extends ArrayIterator
{
	public    $pub2 = 1;
	protected $pro2 = 2;
	private   $pri2 = 3;

	function __construct($ar, $flags = 0)
	{
		echo __METHOD__ . "()\n";
		parent::__construct($ar, $flags);
		$this->imp2 = 4;
	}

	function dump()
	{
		echo __METHOD__ . "()\n";
		var_dump(array('Flags'=>$this->getFlags()
		              ,'OVars'=>get_object_vars($this)
		              ,'$this'=>$this));
	}

	function setFlags($flags)
	{
		echo __METHOD__ . "($flags)\n";
		ArrayIterator::setFlags($flags);
	}
}

class ArrayObjectEx extends ArrayObject
{
	public    $pub1 = 1;
	protected $pro1 = 2;
	private   $pri1 = 3;
	
	function __construct($ar = array(), $flags = 0)
	{
		echo __METHOD__ . "()\n";
		parent::__construct($ar, $flags);
		$this->imp1 = 4;
	}

	function exchange()
	{
		echo __METHOD__ . "()\n";
		$this->exchangeArray($this);
	}

	function dump()
	{
		echo __METHOD__ . "()\n";
		var_dump(array('Flags'=>$this->getFlags()
		              ,'OVars'=>get_object_vars($this)
		              ,'$this'=>$this));
	}

	function show()
	{
		echo __METHOD__ . "()\n";
		foreach($this as $n => $v)
		{
			var_dump(array($n => $v));
		}
	}
	
	function setFlags($flags)
	{
		echo __METHOD__ . "($flags)\n";
		ArrayObject::setFlags($flags);
	}
	
	function getIterator()
	{
		echo __METHOD__ . "()\n";
		$it = new ArrayIteratorEx($this, $this->getFlags());
		$it->dyn2 = 5;
		$it->dump();
		return $it;
	}
}

function check($obj, $flags)
{
	echo "===CHECK===\n";

	$obj->setFlags($flags);
	$obj->dump();
	$obj->show();

	echo "===FOREACH===\n";
	
	$it = $obj->getIterator();
	foreach($it as $n => $v)
	{
		var_dump(array($n => $v));
	}
	
	echo "===PROPERTY===\n";
	
	var_dump($obj->pub1);
	var_dump(isset($obj->a));
	$obj->setFlags($flags | 2);
	var_dump($obj->pub1);
	var_dump(isset($obj->a));
	
	var_dump($it->pub2);
	var_dump(isset($it->pub1));
	$it->setFlags($flags | 2);
	var_dump($it->pub2);
	var_dump(isset($it->pub1));
}

$obj = new ArrayObjectEx(array(0=>1,'a'=>25, 'pub1'=>42), 0);
$obj->dyn1 = 5;

check($obj, 0);
check($obj, 1);

echo "#####EXCHANGE#####\n";

$obj->exchange();

check($obj, 0);
check($obj, 1);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
ArrayObjectEx::__construct()
===CHECK===
ArrayObjectEx::setFlags(0)
ArrayObjectEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(2) {
    [u"a"]=>
    int(25)
    [u"pub1"]=>
    int(42)
  }
  [u"$this"]=>
  object(ArrayObjectEx)#%d (6) {
    [u"pub1"]=>
    int(1)
    [u"pro1":protected]=>
    int(2)
    [u"pri1":u"ArrayObjectEx":private]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
    [u"storage":u"ArrayObject":private]=>
    array(3) {
      [0]=>
      int(1)
      [u"a"]=>
      int(25)
      [u"pub1"]=>
      int(42)
    }
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(2) {
    [u"a"]=>
    int(25)
    [u"pub1"]=>
    int(42)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
      [u"storage":u"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        [u"a"]=>
        int(25)
        [u"pub1"]=>
        int(42)
      }
    }
  }
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [u"a"]=>
  int(25)
}
array(1) {
  [u"pub1"]=>
  int(42)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(2) {
    [u"a"]=>
    int(25)
    [u"pub1"]=>
    int(42)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
      [u"storage":u"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        [u"a"]=>
        int(25)
        [u"pub1"]=>
        int(42)
      }
    }
  }
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [u"a"]=>
  int(25)
}
array(1) {
  [u"pub1"]=>
  int(42)
}
===PROPERTY===
int(1)
bool(false)
ArrayObjectEx::setFlags(2)
int(1)
bool(true)
int(1)
bool(false)
ArrayIteratorEx::setFlags(2)
int(1)
bool(true)
===CHECK===
ArrayObjectEx::setFlags(1)
ArrayObjectEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub1"]=>
    int(1)
    [u"pro1"]=>
    int(2)
    [u"pri1"]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayObjectEx)#%d (6) {
    [u"pub1"]=>
    int(1)
    [u"pro1":protected]=>
    int(2)
    [u"pri1":u"ArrayObjectEx":private]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
    [u"storage":u"ArrayObject":private]=>
    array(3) {
      [0]=>
      int(1)
      [u"a"]=>
      int(25)
      [u"pub1"]=>
      int(42)
    }
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub2"]=>
    int(1)
    [u"pro2"]=>
    int(2)
    [u"pri2"]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
      [u"storage":u"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        [u"a"]=>
        int(25)
        [u"pub1"]=>
        int(42)
      }
    }
  }
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [u"a"]=>
  int(25)
}
array(1) {
  [u"pub1"]=>
  int(42)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub2"]=>
    int(1)
    [u"pro2"]=>
    int(2)
    [u"pri2"]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
      [u"storage":u"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        [u"a"]=>
        int(25)
        [u"pub1"]=>
        int(42)
      }
    }
  }
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [u"a"]=>
  int(25)
}
array(1) {
  [u"pub1"]=>
  int(42)
}
===PROPERTY===
int(1)
bool(false)
ArrayObjectEx::setFlags(3)
int(1)
bool(true)
int(1)
bool(false)
ArrayIteratorEx::setFlags(3)
int(1)
bool(true)
#####EXCHANGE#####
ArrayObjectEx::exchange()
===CHECK===
ArrayObjectEx::setFlags(0)
ArrayObjectEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(5) {
    [u"pub1"]=>
    int(1)
    [u"pro1"]=>
    int(2)
    [u"pri1"]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayObjectEx)#%d (5) {
    [u"pub1"]=>
    int(1)
    [u"pro1":protected]=>
    int(2)
    [u"pri1":u"ArrayObjectEx":private]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(4) {
    [u"pub1"]=>
    int(1)
    [u"pro1"]=>
    int(2)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  [u"pub1"]=>
  int(1)
}
array(1) {
  [u"imp1"]=>
  int(4)
}
array(1) {
  [u"dyn1"]=>
  int(5)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(0)
  [u"OVars"]=>
  array(4) {
    [u"pub1"]=>
    int(1)
    [u"pro1"]=>
    int(2)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  [u"pub1"]=>
  int(1)
}
array(1) {
  [u"imp1"]=>
  int(4)
}
array(1) {
  [u"dyn1"]=>
  int(5)
}
===PROPERTY===
int(1)
bool(false)
ArrayObjectEx::setFlags(2)
int(1)
bool(false)
int(1)
bool(false)
ArrayIteratorEx::setFlags(2)
int(1)
bool(true)
===CHECK===
ArrayObjectEx::setFlags(1)
ArrayObjectEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub1"]=>
    int(1)
    [u"pro1"]=>
    int(2)
    [u"pri1"]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayObjectEx)#%d (5) {
    [u"pub1"]=>
    int(1)
    [u"pro1":protected]=>
    int(2)
    [u"pri1":u"ArrayObjectEx":private]=>
    int(3)
    [u"imp1"]=>
    int(4)
    [u"dyn1"]=>
    int(5)
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub2"]=>
    int(1)
    [u"pro2"]=>
    int(2)
    [u"pri2"]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  [u"pub1"]=>
  int(1)
}
array(1) {
  [u"imp1"]=>
  int(4)
}
array(1) {
  [u"dyn1"]=>
  int(5)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  [u"Flags"]=>
  int(1)
  [u"OVars"]=>
  array(5) {
    [u"pub2"]=>
    int(1)
    [u"pro2"]=>
    int(2)
    [u"pri2"]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
  }
  [u"$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    [u"pub2"]=>
    int(1)
    [u"pro2":protected]=>
    int(2)
    [u"pri2":u"ArrayIteratorEx":private]=>
    int(3)
    [u"imp2"]=>
    int(4)
    [u"dyn2"]=>
    int(5)
    [u"storage":u"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      [u"pub1"]=>
      int(1)
      [u"pro1":protected]=>
      int(2)
      [u"pri1":u"ArrayObjectEx":private]=>
      int(3)
      [u"imp1"]=>
      int(4)
      [u"dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  [u"pub1"]=>
  int(1)
}
array(1) {
  [u"imp1"]=>
  int(4)
}
array(1) {
  [u"dyn1"]=>
  int(5)
}
===PROPERTY===
int(1)
bool(false)
ArrayObjectEx::setFlags(3)
int(1)
bool(false)
int(1)
bool(false)
ArrayIteratorEx::setFlags(3)
int(1)
bool(true)
===DONE===
