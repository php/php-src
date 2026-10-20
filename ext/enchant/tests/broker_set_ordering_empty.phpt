--TEST--
enchant_broker_set_ordering() function - empty tag
--EXTENSIONS--
enchant
--FILE--
<?php
$broker = enchant_broker_init();
try {
    enchant_broker_set_ordering($broker, '', '');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    enchant_broker_set_ordering($broker, '*', '');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
enchant_broker_set_ordering(): Argument #2 ($tag) must not be empty
enchant_broker_set_ordering(): Argument #3 ($ordering) must not be empty
Done
