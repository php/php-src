--TEST--
#38943, properties in extended class cannot be set (5.3+)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
class myZip extends ZipArchive {
    private $test = 0;
    public $testp = 1;
    private $testarray = array();

    public function __construct() {
        $this->testarray[] = 1;
        var_dump($this->testarray);
    }
}

$z = new myZip;
$z->testp = "foobar";
var_dump($z);

?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
object(myZip)#1 (%d) {
  ["test":"myZip":private]=>
  int(0)
  ["testp"]=>
  string(6) "foobar"
  ["testarray":"myZip":private]=>
  array(1) {
    [0]=>
    int(1)
  }
  ["lastId"]=>
  int(-1)
  ["status"]=>
  int(0)
  ["statusSys"]=>
  int(0)
  ["numFiles"]=>
  int(0)
  ["filename"]=>
  string(0) ""
  ["comment"]=>
  string(0) ""
}
