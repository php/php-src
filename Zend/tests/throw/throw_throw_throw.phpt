--TEST--
throw throw guard
--FILE--
<?php

try {
    throw throw new Exception;
} catch (Exception $e) {
}
?>
--EXPECTF--
Fatal error: Cannot throw throw expression in %s on line %d