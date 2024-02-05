--TEST--
Match expression block must not use break
--FILE--
<?php
var_dump(match ($value) {
    1 => { break; },
});
?>
--EXPECTF--
Fatal error: Match expression whose result is used must not contain return, break, continue or goto in %s on line %d
