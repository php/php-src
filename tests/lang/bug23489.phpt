--TEST--
Bug #23489 (ob_start() is broken with method callbacks)
--FILE--
<?php
class Test {
  function __construct() {
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
