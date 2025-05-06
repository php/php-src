--TEST--
Memory leaks
--FILE--
<?php
function t() {
    return new stdClass() + (null ?? return);
    return (null ?? return false);
}
t();
?>
--EXPECT--
