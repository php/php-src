--TEST--
Io\Terminal\Terminal: readKey zero-timeout and sequenceTimeout zero escape sequence decoding
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
if (!class_exists(\Time\Duration::class)) die('skip Time\Duration not available');
?>
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Time\Duration;

if (function_exists('pcntl_alarm')) {
    pcntl_alarm(15);
}

function makeSocketPair(bool $unbuffered = false): array {
    $pair = stream_socket_pair(
        PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
        STREAM_SOCK_STREAM,
        PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
    );
    if ($pair === false) {
        throw new RuntimeException('Failed to create socket pair');
    }
    if ($unbuffered) {
        stream_set_read_buffer($pair[0], 0);
        stream_set_write_buffer($pair[1], 0);
    }
    return $pair;
}

function formatKey(Key|string|null $key): string {
    if ($key instanceof Key) {
        return 'Key::' . $key->name;
    }
    if (is_string($key)) {
        return 'str:' . bin2hex($key);
    }
    return 'null';
}

$zero = Duration::fromSeconds(0);
$posTimeout = Duration::fromMilliseconds(200);
$posSeqTimeout = Duration::fromMilliseconds(400);

foreach ([false, true] as $unbuffered) {
    $modeLabel = $unbuffered ? 'unbuffered' : 'buffered';
    echo "=== Mode: {$modeLabel} ===\n";

    // 1. Complete CSI and SS3 sequences with overall timeout zero
    $sequences = [
        'Up' => "\x1b[A",
        'Down' => "\x1b[B",
        'Right' => "\x1b[C",
        'Left' => "\x1b[D",
        'Insert' => "\x1b[2~",
        'ShiftTab' => "\x1b[Z",
        'F1_SS3' => "\x1bOP",
        'F1_CSI' => "\x1b[11~",
        'Unsupported_CSI' => "\x1b[1;5A",
    ];

    foreach ($sequences as $label => $bytes) {
        [$r, $w] = makeSocketPair($unbuffered);
        $term = Terminal::fromStreams($r);
        fwrite($w, $bytes);
        fflush($w);
        usleep(2000);
        $k = $term->readKey($zero);
        $rem = $term->readKey($zero);
        echo "overall=0 {$label}: " . formatKey($k) . " next=" . formatKey($rem) . "\n";
        fclose($r);
        fclose($w);
    }

    // 2. Consecutive sequences followed by ordinary text, preserving subsequent input
    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    fwrite($w, "\x1b[A\x1b[B\x1b[C\x1b[D\x1b[2~\x1b[Z\x1bOP\x1b[11~ab");
    fflush($w);
    usleep(2000);

    $consecutive = [];
    for ($i = 0; $i < 11; $i++) {
        $consecutive[] = formatKey($term->readKey($zero));
    }
    echo "consecutive overall=0: " . implode(', ', $consecutive) . "\n";
    fclose($r);
    fclose($w);

    // 3. Lone Escape and incomplete prefixes with overall timeout zero (immediate fallback)
    $fallbacks = [
        'Lone_ESC' => "\x1b",
        'Incomplete_CSI_bracket' => "\x1b[",
        'Incomplete_CSI_params' => "\x1b[1;",
        'Incomplete_SS3' => "\x1bO",
    ];
    foreach ($fallbacks as $label => $bytes) {
        [$r, $w] = makeSocketPair($unbuffered);
        $term = Terminal::fromStreams($r);
        fwrite($w, $bytes);
        fflush($w);
        usleep(2000);

        $t0 = hrtime(true);
        $k1 = $term->readKey($zero);
        $elapsedMs = (hrtime(true) - $t0) / 1e6;
        $k2 = $term->readKey($zero);
        echo "fallback overall=0 {$label}: " . formatKey($k1)
            . " next=" . formatKey($k2)
            . " fast=" . ($elapsedMs < 100.0 ? 'true' : 'false') . "\n";
        fclose($r);
        fclose($w);
    }

    // 4. Explicit sequenceTimeout zero with positive overall timeout
    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    fwrite($w, "\x1b[A\x1b[2~\x1b[Z\x1bOP\x1b[11~z");
    fflush($w);
    usleep(2000);

    $seqZeroResults = [];
    for ($i = 0; $i < 6; $i++) {
        $seqZeroResults[] = formatKey($term->readKey($posTimeout, $zero));
    }
    echo "seqTimeout=0 complete: " . implode(', ', $seqZeroResults) . "\n";
    fclose($r);
    fclose($w);

    foreach ($fallbacks as $label => $bytes) {
        [$r, $w] = makeSocketPair($unbuffered);
        $term = Terminal::fromStreams($r);
        fwrite($w, $bytes);
        fflush($w);
        usleep(2000);

        $t0 = hrtime(true);
        $k1 = $term->readKey($posTimeout, $zero);
        $elapsedMs = (hrtime(true) - $t0) / 1e6;
        $k2 = $term->readKey($zero);
        echo "fallback seqTimeout=0 {$label}: " . formatKey($k1)
            . " next=" . formatKey($k2)
            . " fast=" . ($elapsedMs < 100.0 ? 'true' : 'false') . "\n";
        fclose($r);
        fclose($w);
    }

    // 5. Overall zero timeout remains nonblocking even when sequenceTimeout is positive
    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    fwrite($w, "\x1b[A\x1bOPq");
    fflush($w);
    usleep(2000);
    echo "overall=0,seq>0 complete: "
        . formatKey($term->readKey($zero, $posSeqTimeout)) . ", "
        . formatKey($term->readKey($zero, $posSeqTimeout)) . ", "
        . formatKey($term->readKey($zero, $posSeqTimeout)) . "\n";
    fclose($r);
    fclose($w);

    foreach ($fallbacks as $label => $bytes) {
        [$r, $w] = makeSocketPair($unbuffered);
        $term = Terminal::fromStreams($r);
        fwrite($w, $bytes);
        fflush($w);
        usleep(2000);

        $t0 = hrtime(true);
        $k1 = $term->readKey($zero, $posSeqTimeout);
        $elapsedMs = (hrtime(true) - $t0) / 1e6;
        $k2 = $term->readKey($zero, $posSeqTimeout);
        echo "overall=0,seq>0 {$label}: " . formatKey($k1)
            . " next=" . formatKey($k2)
            . " nonblocking=" . ($elapsedMs < 100.0 ? 'true' : 'false') . "\n";
        fclose($r);
        fclose($w);
    }

    // 6. Malformed, overlong (>32 bytes), and invalid UTF-8 / truncated EOF sequences
    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    $overlong = "\x1b[" . str_repeat("1;", 16) . "A!"; // 2 + 32 + 2 = 36 bytes
    fwrite($w, $overlong);
    fflush($w);
    usleep(2000);
    $firstChunk = $term->readKey($zero);
    $rest = [];
    while (($nextKey = $term->readKey($zero)) !== null) {
        $rest[] = $nextKey;
    }
    echo "overlong CSI: len=" . strlen($firstChunk)
        . " prefix_ok=" . (str_starts_with($overlong, $firstChunk) ? 'true' : 'false')
        . " tail=" . implode('', $rest) . "\n";
    fclose($r);
    fclose($w);

    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    fwrite($w, "\x1bx\x1bOX\x80\xff\xe2A");
    fflush($w);
    usleep(2000);
    $malformed = [];
    while (($mk = $term->readKey($zero)) !== null) {
        $malformed[] = formatKey($mk);
    }
    echo "malformed+invalid_utf8: " . implode(', ', $malformed) . "\n";
    fclose($r);
    fclose($w);

    // Fragmented UTF-8 retained on zero-timeout read and completed on next zero-timeout read
    [$r, $w] = makeSocketPair($unbuffered);
    $term = Terminal::fromStreams($r);
    fwrite($w, "\xe2");
    fflush($w);
    usleep(2000);
    $u1 = $term->readKey($zero);
    fwrite($w, "\x82\xac");
    fflush($w);
    usleep(2000);
    $u2 = $term->readKey($zero);
    echo "utf8_retained_zero_timeout: first=" . formatKey($u1) . " second=" . formatKey($u2) . "\n";
    fclose($w);
    fclose($r);
}

// 7. Deterministic bounded fuzzing across random escape/UTF-8/control payloads
mt_srand(0x5EED1234);
$alphabet = ["\x1b", "[", "O", "1", ";", "2", "~", "A", "B", "P", "Z", "\x00", "\x80", "\xc2\xa3", "\xe2\x82\xac", "\xf0\x9f\x98\x80", "\xff", "x"];
$fuzzOk = true;
for ($iter = 0; $iter < 100; $iter++) {
    [$r, $w] = makeSocketPair(($iter % 2) === 1);
    $term = Terminal::fromStreams($r);
    $count = mt_rand(1, 24);
    $payload = '';
    for ($b = 0; $b < $count; $b++) {
        $payload .= $alphabet[mt_rand(0, count($alphabet) - 1)];
    }
    fwrite($w, $payload);
    fflush($w);
    usleep(1000);

    $steps = 0;
    $maxSteps = strlen($payload) + 2;
    while ($steps <= $maxSteps) {
        $steps++;
        $res = $term->readKey($zero);
        if ($res === null) {
            break;
        }
    }
    if ($steps > $maxSteps) {
        $fuzzOk = false;
    }
    fclose($w);
    try {
        $term->readKey($zero);
        $fuzzOk = false;
    } catch (TerminalException) {
        // Expected EOF
    }
    fclose($r);
}
echo "bounded_fuzz_100: " . ($fuzzOk ? 'PASS' : 'FAIL') . "\n";
?>
--EXPECT--
=== Mode: buffered ===
overall=0 Up: Key::Up next=null
overall=0 Down: Key::Down next=null
overall=0 Right: Key::Right next=null
overall=0 Left: Key::Left next=null
overall=0 Insert: Key::Insert next=null
overall=0 ShiftTab: Key::ShiftTab next=null
overall=0 F1_SS3: Key::F1 next=null
overall=0 F1_CSI: Key::F1 next=null
overall=0 Unsupported_CSI: str:1b5b313b3541 next=null
consecutive overall=0: Key::Up, Key::Down, Key::Right, Key::Left, Key::Insert, Key::ShiftTab, Key::F1, Key::F1, str:61, str:62, null
fallback overall=0 Lone_ESC: Key::Escape next=null fast=true
fallback overall=0 Incomplete_CSI_bracket: str:1b5b next=null fast=true
fallback overall=0 Incomplete_CSI_params: str:1b5b313b next=null fast=true
fallback overall=0 Incomplete_SS3: str:1b4f next=null fast=true
seqTimeout=0 complete: Key::Up, Key::Insert, Key::ShiftTab, Key::F1, Key::F1, str:7a
fallback seqTimeout=0 Lone_ESC: Key::Escape next=null fast=true
fallback seqTimeout=0 Incomplete_CSI_bracket: str:1b5b next=null fast=true
fallback seqTimeout=0 Incomplete_CSI_params: str:1b5b313b next=null fast=true
fallback seqTimeout=0 Incomplete_SS3: str:1b4f next=null fast=true
overall=0,seq>0 complete: Key::Up, Key::F1, str:71
overall=0,seq>0 Lone_ESC: Key::Escape next=null nonblocking=true
overall=0,seq>0 Incomplete_CSI_bracket: str:1b5b next=null nonblocking=true
overall=0,seq>0 Incomplete_CSI_params: str:1b5b313b next=null nonblocking=true
overall=0,seq>0 Incomplete_SS3: str:1b4f next=null nonblocking=true
overlong CSI: len=32 prefix_ok=true tail=1;A!
malformed+invalid_utf8: str:1b78, str:1b4f58, str:80, str:ff, str:e241
utf8_retained_zero_timeout: first=null second=str:e282ac
=== Mode: unbuffered ===
overall=0 Up: Key::Up next=null
overall=0 Down: Key::Down next=null
overall=0 Right: Key::Right next=null
overall=0 Left: Key::Left next=null
overall=0 Insert: Key::Insert next=null
overall=0 ShiftTab: Key::ShiftTab next=null
overall=0 F1_SS3: Key::F1 next=null
overall=0 F1_CSI: Key::F1 next=null
overall=0 Unsupported_CSI: str:1b5b313b3541 next=null
consecutive overall=0: Key::Up, Key::Down, Key::Right, Key::Left, Key::Insert, Key::ShiftTab, Key::F1, Key::F1, str:61, str:62, null
fallback overall=0 Lone_ESC: Key::Escape next=null fast=true
fallback overall=0 Incomplete_CSI_bracket: str:1b5b next=null fast=true
fallback overall=0 Incomplete_CSI_params: str:1b5b313b next=null fast=true
fallback overall=0 Incomplete_SS3: str:1b4f next=null fast=true
seqTimeout=0 complete: Key::Up, Key::Insert, Key::ShiftTab, Key::F1, Key::F1, str:7a
fallback seqTimeout=0 Lone_ESC: Key::Escape next=null fast=true
fallback seqTimeout=0 Incomplete_CSI_bracket: str:1b5b next=null fast=true
fallback seqTimeout=0 Incomplete_CSI_params: str:1b5b313b next=null fast=true
fallback seqTimeout=0 Incomplete_SS3: str:1b4f next=null fast=true
overall=0,seq>0 complete: Key::Up, Key::F1, str:71
overall=0,seq>0 Lone_ESC: Key::Escape next=null nonblocking=true
overall=0,seq>0 Incomplete_CSI_bracket: str:1b5b next=null nonblocking=true
overall=0,seq>0 Incomplete_CSI_params: str:1b5b313b next=null nonblocking=true
overall=0,seq>0 Incomplete_SS3: str:1b4f next=null nonblocking=true
overlong CSI: len=32 prefix_ok=true tail=1;A!
malformed+invalid_utf8: str:1b78, str:1b4f58, str:80, str:ff, str:e241
utf8_retained_zero_timeout: first=null second=str:e282ac
bounded_fuzz_100: PASS
