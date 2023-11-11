--TEST--
GH-8318 (SplFileObject useless call to an internal method for erroring)
--FILE--
<?php
class bug8318 extends \SplFileObject
{
    public function __construct()
    {
    }

    public function fpassthru(): int
    {
        return 0;
    }
}

$cl = new bug8318;
try {
    $cl->fpassthru();
} catch (\Error $e) {
    var_dump($e);
}
?>
--EXPECTF--
object(Error)#2 (7) {
  ["message":protected]=>
  string(72) "The parent constructor was not called: the object is in an invalid state"
  ["string":"Error":private]=>
  string(0) ""
  ["code":protected]=>
  int(0)
  ["file":protected]=>
  string(%d) "%s"
  ["line":protected]=>
  int(16)
  ["trace":"Error":private]=>
  array(0) {
  }
  ["previous":"Error":private]=>
  NULL
}
