--TEST--
Early break
--FILE--
<?php
for($i = 5; $i >= 0; $i--) {
    try {
        echo (function(){ return $var = $i ?: continue; })($i);
    } catch (\Error $e) {
        var_dump($e->getMessage());
    }
}
?>
--EXPECTF--
Fatal error: 'continue' not in the 'loop' or 'switch' context in %s on line %d
