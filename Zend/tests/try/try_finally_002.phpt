--TEST--
Try finally (re-throw exception in finally block)
--FILE--
<?php
function foo () {
   try {
     throw new Exception("try");
   } finally {
     throw new Exception("finally");
   }
}

try {
  foo();
} catch (Exception $e) {
  do {
    echo $e::class, ': ', $e->getMessage(), "\n";
  } while ($e = $e->getPrevious());
}
?>
--EXPECT--
Exception: finally
Exception: try
