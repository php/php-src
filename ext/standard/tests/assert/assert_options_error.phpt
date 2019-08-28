--TEST--
assert_options() - unknown assert option.

--FILE--
<?php
try {
    assert_options(1000);
} catch (\Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Unknown value 1000