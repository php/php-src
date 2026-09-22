--TEST--
enchant_broker_dict_exists() function - empty tag
--EXTENSIONS--
enchant
--FILE--
<?php
$broker = enchant_broker_init();
try {
    enchant_broker_dict_exists($broker, '');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
echo "Done\n";
?>
--EXPECT--
enchant_broker_dict_exists(): Argument #2 ($tag) must not be empty
Done
