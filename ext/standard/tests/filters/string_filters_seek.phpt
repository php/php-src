--TEST--
string filters (rot13, toupper, tolower) with seek - fully seekable
--FILE--
<?php
$file = __DIR__ . '/string_filters_seek.txt';

$text = 'Hello World! This is a test for string filter seeking.';

$filters = ['string.rot13', 'string.toupper', 'string.tolower'];
$expected = [
    'string.rot13' => 'Uryyb Jbeyq! Guvf vf n grfg sbe fgevat svygre frrxvat.',
    'string.toupper' => 'HELLO WORLD! THIS IS A TEST FOR STRING FILTER SEEKING.',
    'string.tolower' => 'hello world! this is a test for string filter seeking.'
];

foreach ($filters as $filter) {
    echo "Testing filter: $filter\n";
    
    file_put_contents($file, $text);
    
    $fp = fopen($file, 'r');
    stream_filter_append($fp, $filter, STREAM_FILTER_READ);
    
    $partial = fread($fp, 20);
    echo "First read (20 bytes): $partial\n";
    
    $result = fseek($fp, 0, SEEK_SET);
    echo "Seek to start: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";
    
    $full = fread($fp, strlen($text));
    echo "Content matches: " . ($full === $expected[$filter] ? "YES" : "NO") . "\n";
    
    $result = fseek($fp, 13, SEEK_SET);
    echo "Seek to middle: " . ($result === 0 ? "SUCCESS" : "FAILURE") . "\n";
    
    $from_middle = fread($fp, 10);
    $expected_middle = substr($expected[$filter], 13, 10);
    echo "Read from middle matches: " . ($from_middle === $expected_middle ? "YES" : "NO") . "\n";
    
    fclose($fp);
    echo "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/string_filters_seek.txt');
?>
--EXPECT--
Testing filter: string.rot13
First read (20 bytes): Uryyb Jbeyq! Guvf vf
Seek to start: SUCCESS
Content matches: YES
Seek to middle: SUCCESS
Read from middle matches: YES

Testing filter: string.toupper
First read (20 bytes): HELLO WORLD! THIS IS
Seek to start: SUCCESS
Content matches: YES
Seek to middle: SUCCESS
Read from middle matches: YES

Testing filter: string.tolower
First read (20 bytes): hello world! this is
Seek to start: SUCCESS
Content matches: YES
Seek to middle: SUCCESS
Read from middle matches: YES
