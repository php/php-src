--TEST--
SQLite3::open test with empty string argument via the constructor
--CREDITS--
Thijs Feryn <thijs@feryn.eu>
#TestFest PHPBelgium 2009
--FILE--
<?php
try{
    $db = new SQLite3('');
} catch(Exception $e) {
    echo $e->getMessage().PHP_EOL;
}
echo "Done\n";
?>
--EXPECTF--
Unable to expand filepath
Done
