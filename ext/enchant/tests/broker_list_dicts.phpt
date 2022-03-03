--TEST--
enchant_broker_list_dicts() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--EXTENSIONS--
enchant
--SKIPIF--
<?php
$broker = enchant_broker_init();

if (!$broker) {
    echo "skip: Unable to init broker\n";
    exit;
}

if (!enchant_broker_list_dicts($broker)) {
    @enchant_broker_free($broker);

    echo "skip: No broker dicts installed\n";
}

@enchant_broker_free($broker);
?>
--FILE--
<?php
$broker = enchant_broker_init();
if (is_object($broker)) {
    echo("OK\n");
    $brokerListDicts = enchant_broker_list_dicts($broker);

    if (is_array($brokerListDicts)) {
        echo("OK\n");
    } else {
        echo("broker list dicts failed\n");
    }
} else {
    echo("init failed\n");
}
?>
--EXPECT--
OK
OK
