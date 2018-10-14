--TEST--
Bug #30362 (stream_get_line() not working as documented)
--FILE--
<?php

$resource = fopen(dirname(__FILE__).'/bug30362.txt', 'rb');

for ($i = 0; ($i < 10) && !feof($resource); ++$i ) {
    $a = "Y";
    $line = stream_get_line($resource, 50, $a);
    echo $line . "\n";
}
fclose($resource);

?>
--EXPECT--
111
111111111
111111111
111111111
111111111
111111111
111111111
111111111
111111111
111111111
