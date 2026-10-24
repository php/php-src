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

function stageLineHandoff(string $continuation, bool $closeWriter = false): array {
    [$read, $write] = stream_socket_pair(
        PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
        STREAM_SOCK_STREAM,
        PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
    );
    fwrite($write, "\xE2");
    $term = Terminal::fromStreams($read);
    // Writer remains open; readKey times out after 20ms and retains \xE2
    $k = $term->readKey(Duration::fromMilliseconds(20));
    if ($continuation !== "") {
        fwrite($write, $continuation);
    }
    if ($closeWriter) {
        fclose($write);
        $write = null;
    }
    return [$term, $read, $write];
}

// 1. Complete UTF-8 sequence: \xE2 + \x82\xACline\n => €line
[$t1, $read1, $write1] = stageLineHandoff("\x82\xACline\n");
$line1 = $t1->readLine();
var_dump($line1 === "€line");
fclose($read1);
if ($write1) fclose($write1);

// 2. Incomplete UTF-8 at EOF: \xE2 at EOF returns pending byte
[$t2, $read2, $write2] = stageLineHandoff("", true);
$line2 = $t2->readLine();
var_dump($line2 === "\xE2");
fclose($read2);
if ($write2) fclose($write2);

// 3. Malformed UTF-8: \xE2 followed by non-continuation ASCII
[$t3, $read3, $write3] = stageLineHandoff("Xline\n");
$line3 = $t3->readLine();
var_dump($line3 === "\xE2Xline");
fclose($read3);
if ($write3) fclose($write3);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
