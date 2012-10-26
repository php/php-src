--TEST--
ZE2 Tests that a getter/setter works properly with all operators 
--FILE--
<?php

class SampleClass {
}

class TimePeriod {
	public $Seconds = 10800;
	public $_aString = 'abc';
	public $_anArray = array('a' => 1, 'b' => 2);
	public $_anObject = NULL;
	
	public function __construct() {
		$this->_anObject = new SampleClass();
	}
	
	public $Hours {
		get { return $this->Seconds / 3600; }
		set { $this->Seconds = $value * 3600; }
	}
	public $aString {
		get { return $this->_aString; }
		set { $this->_aString = $value; }
	}
	public $anArray {
		get { return $this->_anArray; }
		set { $this->_anArray = $value; }
	}
	public $anObject {
		get { return $this->_anObject; }
		set { $this->_anObject = $value; }
	}
}

$o = new TimePeriod();

echo "\n--- Numeric Tests ---\n";
echo "\$o->Hours = ".$o->Hours."\n";
echo "\$o->Hours++: ".($o->Hours++)."\n";
echo "After: {$o->Hours}\n";
echo "\$o->Hours--: ".($o->Hours--)."\n";
echo "After: {$o->Hours}\n";
echo "++\$o->Hours: ".(++$o->Hours)."\n";
echo "--\$o->Hours: ".(--$o->Hours)."\n";
$o->Hours += 2;
echo "\$o->Hours += 2: {$o->Hours}\n";
$o->Hours -= 2;
echo "\$o->Hours -= 2: {$o->Hours}\n";
$o->Hours *= 3;
echo "\$o->Hours *= 3: {$o->Hours}\n";
$o->Hours /= 3;
echo "\$o->Hours /= 3: {$o->Hours}\n";
$o->Hours &= 2;
echo "\$o->Hours &= 2: {$o->Hours}\n";
$o->Hours = 3;	/* Reset */
echo "\$o->Hours = ".$o->Hours."\n";
$o->Hours |= 2;
echo "\$o->Hours |= 2: {$o->Hours}\n";
echo "\$o->Hours % 3: ".($o->Hours % 3)."\n";
echo "\$o->Hours << 3: ".($o->Hours << 3)."\n";
echo "\$o->Hours >> 1: ".($o->Hours >> 1)."\n";
echo "-\$o->Hours : ".(-$o->Hours)."\n";
echo "\$o->Hours | 2: ".($o->Hours | 2)."\n";
echo "\$o->Hours & 2: ".($o->Hours & 2)."\n";
echo "\$o->Hours ^ 2: ".($o->Hours ^ 2)."\n";
echo "~\$o->Hours: ".(~ $o->Hours)."\n";
echo "\$o->Hours == 3: ".(int)($o->Hours == 3)."\n";
echo "\$o->Hours === 3: ".(int)($o->Hours === 3)."\n";
echo "\$o->Hours != 3: ".(int)($o->Hours != 3)."\n";
echo "\$o->Hours <> 3: ".(int)($o->Hours <> 3)."\n";
echo "\$o->Hours !== 3: ".(int)($o->Hours !== 3)."\n";
echo "\$o->Hours < 4: ".(int)($o->Hours < 4)."\n";
echo "\$o->Hours > 4: ".(int)($o->Hours > 4)."\n";
echo "\$o->Hours <= 3: ".(int)($o->Hours <= 3)."\n";
echo "\$o->Hours >= 3: ".(int)($o->Hours >= 3)."\n";
echo "\$o->Hours && 3: ".(int)($o->Hours && 3)."\n";
echo "\$o->Hours || 3: ".(int)($o->Hours || 3)."\n";
echo "\$o->Hours xor 3: ".(int)($o->Hours xor 3)."\n";
echo "\n--- String Tests ---\n";
echo "\$o->aString = \"".($o->aString)."\"\n";
echo "\$o->aString.\"def\": \"".($o->aString . "def")."\"\n";
$o->aString .= 'ghi';
echo "\$o->aString .= \"ghi\": \"".($o->aString)."\"\n";
echo "\n--- Array Tests ---\n";
echo "\$o->anArray = array(".implode(',', $o->anArray).")\n";
echo "\$o->anArray + array('c' => 3, 'd' => 4) = array(".implode(',', $o->anArray + array('c' => 3,'d' => 4)).")\n";
echo "\$o->anArray == array('a' => 1, 'b' => 2) = ".(int)($o->anArray == array('a' => 1, 'b' => 2))."\n";
echo "\$o->anArray != array('a' => 1, 'b' => 2) = ".(int)($o->anArray != array('a' => 1, 'b' => 2))."\n";
echo "\$o->anArray <> array('a' => 1, 'b' => 2) = ".(int)($o->anArray <> array('a' => 1, 'b' => 2))."\n";
echo "\$o->anArray === array('a' => 1, 'b' => 2) = ".(int)($o->anArray === array('a' => 1, 'b' => 2))."\n";
echo "\$o->anArray !== array('a' => 1, 'b' => 2) = ".(int)($o->anArray !== array('a' => 1, 'b' => 2))."\n";
echo "\n--- Type Tests ---\n";
echo "get_class(\$o->anObject) = ".get_class($o->anObject)."\n";
echo "\$o->anObject instanceof SampleClass ".($o->anObject instanceof SampleClass)."\n";
?>
--EXPECTF--
--- Numeric Tests ---
$o->Hours = 3
$o->Hours++: 3
After: 4
$o->Hours--: 4
After: 3
++$o->Hours: 4
--$o->Hours: 3
$o->Hours += 2: 5
$o->Hours -= 2: 3
$o->Hours *= 3: 9
$o->Hours /= 3: 3
$o->Hours &= 2: 2
$o->Hours = 3
$o->Hours |= 2: 3
$o->Hours % 3: 0
$o->Hours << 3: 24
$o->Hours >> 1: 1
-$o->Hours : -3
$o->Hours | 2: 3
$o->Hours & 2: 2
$o->Hours ^ 2: 1
~$o->Hours: -4
$o->Hours == 3: 1
$o->Hours === 3: 1
$o->Hours != 3: 0
$o->Hours <> 3: 0
$o->Hours !== 3: 0
$o->Hours < 4: 1
$o->Hours > 4: 0
$o->Hours <= 3: 1
$o->Hours >= 3: 1
$o->Hours && 3: 1
$o->Hours || 3: 1
$o->Hours xor 3: 0

--- String Tests ---
$o->aString = "abc"
$o->aString."def": "abcdef"
$o->aString .= "ghi": "abcghi"

--- Array Tests ---
$o->anArray = array(1,2)
$o->anArray + array('c' => 3, 'd' => 4) = array(1,2,3,4)
$o->anArray == array('a' => 1, 'b' => 2) = 1
$o->anArray != array('a' => 1, 'b' => 2) = 0
$o->anArray <> array('a' => 1, 'b' => 2) = 0
$o->anArray === array('a' => 1, 'b' => 2) = 1
$o->anArray !== array('a' => 1, 'b' => 2) = 0

--- Type Tests ---
get_class($o->anObject) = SampleClass
$o->anObject instanceof SampleClass 1
