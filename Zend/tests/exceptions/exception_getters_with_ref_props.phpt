--TEST--
Calling exception getters when properties hold references
--FILE--
<?php

class MyException extends Exception {
    public function __construct(&$refMsg, &$refCode, &$refFile, &$refLine) {
        $this->message =& $refMsg;
        $this->code =& $refCode;
        $this->file =& $refFile;
        $this->line =& $refLine;
    }
}

$refMsg = "foo";
$refCode = 0;
$refFile = "foobar";
$refLine = 42;
$ex = new MyException($refMsg, $refCode, $refFile, $refLine);
var_dump($ex->getMessage());
var_dump($ex->getCode());
var_dump($ex->getFile());
var_dump($ex->getLine());

?>
--EXPECT--
string(3) "foo"
int(0)
string(6) "foobar"
int(42)
