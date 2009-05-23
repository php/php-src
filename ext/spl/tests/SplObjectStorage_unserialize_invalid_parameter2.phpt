--TEST--
Check that SplObjectStorage::unserialize throws exception when numeric value passed
--CREDITS--
PHPNW Testfest 2009 - Simon Westcott (swestcott@gmail.com)
--FILE--
<?php

$data_provider = array(
    12345,
    1.2345,
    PHP_INT_MAX,
    'x:rubbish', // rubbish after the 'x:' prefix
    'x:i:2;O:8:"stdClass":0:{},s:5:"value";;m:a:0:{}',
);

foreach($data_provider as $input) {

	$s = new SplObjectStorage();

    try {
        $s->unserialize($input);
    } catch(UnexpectedValueException $e) {
        echo $e->getMessage() . PHP_EOL;
    }
}

?>
--EXPECTF--
Error at offset %d of %d bytes
Error at offset %d of %d bytes
Error at offset %d of %d bytes
Error at offset %d of %d bytes
Error at offset %d of %d bytes

