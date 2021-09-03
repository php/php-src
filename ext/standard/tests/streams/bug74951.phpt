--TEST--
Bug#74951 Null pointer dereference in user streams
--FILE--
<?php
trait Stream00ploiter{
  public function s() {}
  public function n($_) {}
}
stream_wrapper_register('e0ploit','Stream00ploiter');
$s=fopen('e0ploit://',0);
?>
--EXPECTF--
Warning: fopen(e0ploit://): Failed to open stream: operation failed in %s%ebug74951.php on line 7
