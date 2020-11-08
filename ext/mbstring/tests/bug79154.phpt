--TEST--
Bug 79154 (mb_convert_encoding() can modify $from_encoding)
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('mbstring extension not available');
?>
--FILE--
<?php
class Utf8Encoding
{
    public function __toString()
    {
        return 'UTF-8';
    }
}

$utf8encoding = new Utf8Encoding();
$encodings = [$utf8encoding];
var_dump($encodings);
mb_convert_encoding('foo', 'UTF-8', $encodings);
var_dump($encodings);

?>
--EXPECTF--
array(1) {
  [0]=>
  object(Utf8Encoding)#%d (0) {
  }
}
array(1) {
  [0]=>
  object(Utf8Encoding)#%d (0) {
  }
}
