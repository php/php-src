--TEST--
Check that SplArray::fromArray will not allow integer overflows
--CREDITS--
Rob Knight <themanhimself@robknight.org.uk> PHPNW Test Fest 2009
--FILE--
<?php
$array = array(PHP_INT_MAX => 'foo');
$splArray = new SplFixedArray();

try {
  $splArray->fromArray($array);
} catch (Error $e) {
  echo $e->getMessage();
}
?>
--EXPECT--
Integer overflow detected
