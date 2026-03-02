--TEST--
Hash: Attempt to instantiate a HashContext directly
--FILE--
<?php

try {
  new HashContext;
} catch (Error $e) {
  echo "Exception: {$e->getMessage()}\n";
}
?>
--EXPECT--
Exception: Call to private HashContext::__construct() from global scope
