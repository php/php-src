--TEST--
Bug #70513: GMP Deserialization Type Confusion Vulnerability
--EXTENSIONS--
gmp
--FILE--
<?php

class obj
{
    var $ryat;

    function __wakeup()
    {
        $this->ryat = 1;
    }
}

$obj = new stdClass;
$obj->aa = 1;
$obj->bb = 2;

$inner = 's:1:"1";a:3:{s:2:"aa";s:2:"hi";s:2:"bb";s:2:"hi";i:0;O:3:"obj":1:{s:4:"ryat";R:2;}}';
$exploit = 'a:1:{i:0;C:3:"GMP":'.strlen($inner).':{'.$inner.'}}';
$x = unserialize($exploit);
var_dump($x);
var_dump($obj);

?>
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
object(stdClass)#1 (2) {
  ["aa"]=>
  int(1)
  ["bb"]=>
  int(2)
}
