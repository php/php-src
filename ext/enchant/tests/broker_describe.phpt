--TEST--
enchant_broker_describe() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if (!extension_loaded('enchant')) {
	echo "skip: Enchant extension not enabled\n";
	exit;
}

$broker = enchant_broker_init();

if (!$broker) {
	echo "skip: Unable to init broker\n";
	exit;
}

if (!enchant_broker_describe($broker)) {
	enchant_broker_free($broker);

	echo "skip: No broker providers found\n";
}

enchant_broker_free($broker);
?>
--FILE--
<?php
$broker = enchant_broker_init();
$counter = 0;

if($broker) {
    echo("OK\n");
    $provider = enchant_broker_describe($broker);

    if (is_array($provider)) {
        echo("OK\n");

	if ((isset($provider[$counter]['name']) && isset($provider[$counter]['desc']) && isset($provider[$counter]['file']))) {
	   echo("OK\n");

	} else {
	   echo("failed, broker describe\n");

	}
    } else {
        echo "failed, brocker describe array \n";
    }

    enchant_broker_free($broker);

} else {
    echo("failed, broker_init failure\n");
}
?>
--EXPECT--
OK
OK
OK
