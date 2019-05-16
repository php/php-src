--TEST--
Bug #39944 (References broken)
--FILE--
<?php
$intTheValue = 0;

function &getValue() {
    global $intTheValue;
    return $intTheValue;
}

function setValue(&$int, $iNewValue) {
    $int = $iNewValue;
}

setValue(getValue(), 10);
echo "intTheValue = {$intTheValue}\n";

$b = &$intTheValue;

setValue(getValue(), 10);
echo "intTheValue = {$intTheValue}\n";

/****/

$arrTheArray = array();

function &getArray() {
	global $arrTheArray;
	return $arrTheArray;
}

function addToArray(&$arr, $strToAdd) {
	$arr[] = $strToAdd;
}

addToArray(getArray(), "xx1");
$a = getArray();
addToArray($a, "xx2");
$b = &$arrTheArray;
addToArray($b, "xx3");
addToArray(getArray(), "xx4");
$a = getArray();
addToArray($a, "xx5");
echo "arrTheArray = " . print_r($arrTheArray, 1);

/****/

class RefTest {
	protected $arr;

	function Add($strToAdd) {
		$this->addToArray($this->getArray(), $strToAdd);
	}

	function &getArray() {
		if (!$this->arr)
			$this->arr = array();
		return $this->arr;
	}

	private function addToArray(&$arr, $strToAdd) {
		$arr[] = $strToAdd;
	}
}

$objRefTest = new RefTest();
$objRefTest->Add("xx1");
$objRefTest->Add("xx2");
$objRefTest->Add("xx3");

echo "objRefTest->getArray() = " . print_r($objRefTest->getArray(), 1);
?>
--EXPECT--
intTheValue = 10
intTheValue = 10
arrTheArray = Array
(
    [0] => xx1
    [1] => xx3
    [2] => xx4
)
objRefTest->getArray() = Array
(
    [0] => xx1
    [1] => xx2
    [2] => xx3
)
