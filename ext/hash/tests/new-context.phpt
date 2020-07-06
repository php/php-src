--TEST--
Hash: Attempt to instantiate a HashContext directly
--FILE--
<?php

try {
  new HashContext;
} catch (Error $e) {
  echo "Exception: {$e->getMessage()}\n";
}
--EXPECT--
Exception: Private method HashContext::__construct() cannot be called from the global scope
