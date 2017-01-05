--TEST--
Iterator exceptions in foreach by reference
--FILE--
<?php
class IT extends ArrayIterator {
    private $n = 0;

    function __construct($trap = null) {
    	parent::__construct([0, 1]);
    	$this->trap = $trap;
    }

    function trap($trap) {
    	if ($trap === $this->trap) {
    		throw new Exception($trap);
    	}
    }

    function rewind()  {$this->trap(__FUNCTION__); return parent::rewind();}
    function valid()   {$this->trap(__FUNCTION__); return parent::valid();}
    function key()     {$this->trap(__FUNCTION__); return parent::key();}
    function next()    {$this->trap(__FUNCTION__); return parent::next();}
}

foreach(['rewind', 'valid', 'key', 'next'] as $trap) {
	$obj = new IT($trap);
	try {
		// IS_CV
		foreach ($obj as $key => &$val) echo "$val\n";
	} catch (Exception $e) {
		echo $e->getMessage() . "\n";
	}
	unset($obj);

	try {
		// IS_VAR
		foreach (new IT($trap) as $key => &$val) echo "$val\n";
	} catch (Exception $e) {
		echo $e->getMessage() . "\n";
	}

	try {
		// IS_TMP_VAR
		foreach ((object)new IT($trap) as $key => &$val) echo "$val\n";
	} catch (Exception $e) {
		echo $e->getMessage() . "\n";
	}
}
?>
--EXPECT--
rewind
rewind
rewind
valid
valid
valid
key
key
key
0
next
0
next
0
next
