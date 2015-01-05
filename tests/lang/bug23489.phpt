--TEST--
Bug #23489 (ob_start() is broken with method callbacks)
--FILE--
<?php
class Test {
  function Test() {
    ob_start(
      array(
        $this, 'transform'
      )
    );
  }

  function transform($buffer) {
    return 'success';
  }
}

$t = new Test;
?>
failure
--EXPECT--
success
