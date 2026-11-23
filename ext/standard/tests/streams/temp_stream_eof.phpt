--TEST--
php://temp must not report EOF before a read hits the end of the data
--FILE--
<?php
foreach (['php://memory', 'php://temp', 'php://temp/maxmemory:2'] as $url) {
    $h = fopen($url, 'w+');
    fwrite($h, "a\nb\nc\n");
    rewind($h);
    echo $url, ":";
    while (!feof($h)) {
        $line = fgets($h);
        echo " ", json_encode($line), feof($h) ? "(EOF)" : "";
    }
    echo "\n";
    fclose($h);
}
?>
--EXPECT--
php://memory: "a\n" "b\n" "c\n" false(EOF)
php://temp: "a\n" "b\n" "c\n" false(EOF)
php://temp/maxmemory:2: "a\n" "b\n" "c\n" false(EOF)
