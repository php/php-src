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
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(2) {
    ["a"]=>
    int(25)
    ["pub1"]=>
    int(42)
  }
  ["$this"]=>
  object(ArrayObjectEx)#%d (6) {
    ["pub1"]=>
    int(1)
    ["pro1":protected]=>
    int(2)
    ["pri1":"ArrayObjectEx":private]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
    ["storage":"ArrayObject":private]=>
    array(3) {
      [0]=>
      int(1)
      ["a"]=>
      int(25)
      ["pub1"]=>
      int(42)
    }
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(2) {
    ["a"]=>
    int(25)
    ["pub1"]=>
    int(42)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
      ["storage":"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        ["a"]=>
        int(25)
        ["pub1"]=>
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
  ["a"]=>
  int(25)
}
array(1) {
  ["pub1"]=>
  int(42)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(2) {
    ["a"]=>
    int(25)
    ["pub1"]=>
    int(42)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
      ["storage":"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        ["a"]=>
        int(25)
        ["pub1"]=>
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
  ["a"]=>
  int(25)
}
array(1) {
  ["pub1"]=>
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
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub1"]=>
    int(1)
    ["pro1"]=>
    int(2)
    ["pri1"]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayObjectEx)#%d (6) {
    ["pub1"]=>
    int(1)
    ["pro1":protected]=>
    int(2)
    ["pri1":"ArrayObjectEx":private]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
    ["storage":"ArrayObject":private]=>
    array(3) {
      [0]=>
      int(1)
      ["a"]=>
      int(25)
      ["pub1"]=>
      int(42)
    }
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub2"]=>
    int(1)
    ["pro2"]=>
    int(2)
    ["pri2"]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
      ["storage":"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        ["a"]=>
        int(25)
        ["pub1"]=>
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
  ["a"]=>
  int(25)
}
array(1) {
  ["pub1"]=>
  int(42)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub2"]=>
    int(1)
    ["pro2"]=>
    int(2)
    ["pri2"]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (6) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
      ["storage":"ArrayObject":private]=>
      array(3) {
        [0]=>
        int(1)
        ["a"]=>
        int(25)
        ["pub1"]=>
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
  ["a"]=>
  int(25)
}
array(1) {
  ["pub1"]=>
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
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(5) {
    ["pub1"]=>
    int(1)
    ["pro1"]=>
    int(2)
    ["pri1"]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayObjectEx)#%d (5) {
    ["pub1"]=>
    int(1)
    ["pro1":protected]=>
    int(2)
    ["pri1":"ArrayObjectEx":private]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(4) {
    ["pub1"]=>
    int(1)
    ["pro1"]=>
    int(2)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  ["pub1"]=>
  int(1)
}
array(1) {
  ["imp1"]=>
  int(4)
}
array(1) {
  ["dyn1"]=>
  int(5)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(0)
  ["OVars"]=>
  array(4) {
    ["pub1"]=>
    int(1)
    ["pro1"]=>
    int(2)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  ["pub1"]=>
  int(1)
}
array(1) {
  ["imp1"]=>
  int(4)
}
array(1) {
  ["dyn1"]=>
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
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub1"]=>
    int(1)
    ["pro1"]=>
    int(2)
    ["pri1"]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayObjectEx)#%d (5) {
    ["pub1"]=>
    int(1)
    ["pro1":protected]=>
    int(2)
    ["pri1":"ArrayObjectEx":private]=>
    int(3)
    ["imp1"]=>
    int(4)
    ["dyn1"]=>
    int(5)
  }
}
ArrayObjectEx::show()
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub2"]=>
    int(1)
    ["pro2"]=>
    int(2)
    ["pri2"]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  ["pub1"]=>
  int(1)
}
array(1) {
  ["imp1"]=>
  int(4)
}
array(1) {
  ["dyn1"]=>
  int(5)
}
===FOREACH===
ArrayObjectEx::getIterator()
ArrayIteratorEx::__construct()
ArrayIteratorEx::dump()
array(3) {
  ["Flags"]=>
  int(1)
  ["OVars"]=>
  array(5) {
    ["pub2"]=>
    int(1)
    ["pro2"]=>
    int(2)
    ["pri2"]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
  }
  ["$this"]=>
  object(ArrayIteratorEx)#%d (6) {
    ["pub2"]=>
    int(1)
    ["pro2":protected]=>
    int(2)
    ["pri2":"ArrayIteratorEx":private]=>
    int(3)
    ["imp2"]=>
    int(4)
    ["dyn2"]=>
    int(5)
    ["storage":"ArrayIterator":private]=>
    object(ArrayObjectEx)#%d (5) {
      ["pub1"]=>
      int(1)
      ["pro1":protected]=>
      int(2)
      ["pri1":"ArrayObjectEx":private]=>
      int(3)
      ["imp1"]=>
      int(4)
      ["dyn1"]=>
      int(5)
    }
  }
}
array(1) {
  ["pub1"]=>
  int(1)
}
array(1) {
  ["imp1"]=>
  int(4)
}
array(1) {
  ["dyn1"]=>
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
