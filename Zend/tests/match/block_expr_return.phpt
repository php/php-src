--TEST--
Match expression block must not use return
--FILE--
<?php
var_dump(match ($value) {
    1 => { return; },
});
?>
--EXPECTF--
Fatal error: Match expression whose result is used must not contain return, break, continue or goto in %s on line %d
