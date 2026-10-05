--TEST--
Io\Terminal\Terminal: pending UTF-8 handoff from readKey to readLine
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

function stageLineHandoff(string $continuation): array {
    $fp = fopen('php://memory', 'r+');
    fwrite($fp, "\xE2");
    rewind($fp);
    $term = Terminal::fromStreams($fp);
    $k = $term->readKey(Duration::fromSeconds(0));
    fwrite($fp, $continuation);
    fseek($fp, 1);
    return [$term, $fp];
}

// 1. Complete UTF-8 sequence: \xE2 + \x82\xACline\n => €line
[$t1, $fp1] = stageLineHandoff("\x82\xACline\n");
$line1 = $t1->readLine();
var_dump($line1 === "€line");
fclose($fp1);

// 2. Incomplete UTF-8 at EOF: \xE2 at EOF returns pending byte
[$t2, $fp2] = stageLineHandoff("");
$line2 = $t2->readLine();
var_dump($line2 === "\xE2");
fclose($fp2);

// 3. Malformed UTF-8: \xE2 followed by non-continuation ASCII
[$t3, $fp3] = stageLineHandoff("Xline\n");
$line3 = $t3->readLine();
var_dump($line3 === "\xE2Xline");
fclose($fp3);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
