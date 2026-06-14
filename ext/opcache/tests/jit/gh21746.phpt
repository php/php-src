--TEST--
GH-21746: Segfault with tracing JIT on 2nd call of eval'd closure
--CREDITS--
theodorejb
--EXTENSIONS--
opcache
--INI--
opcache.jit=tracing
--FILE--
<?php

final class LR {
    public static function p(mixed $cx, string $name, mixed $context, array $hash, string $indent): string {
        try { $result = ''; } finally {}
        $lines = explode("\n", $result);
        foreach ($lines as $i => $_) { if ($i === 0) break; }
        return $result;
    }
    public static function hbbch(array $positional, ?\Closure $cb): string {
        if ($cb && is_array($positional[0] ?? null)) foreach ($positional[0] as $v) $cb($v);
        return '';
    }
}

$renderFile = __DIR__ . '/gh21746.inc';

// prevent JITing $renderFile
ini_set('opcache.file_update_protection', 100);
touch($renderFile);

$renderer = require $renderFile;

for ($r = 0; $r < 70; $r++) {
    echo "Render $r...";
    $renderer();
    echo "OK\n";
}

?>
==DONE==
--EXPECT--
Render 0...OK
Render 1...OK
Render 2...OK
Render 3...OK
Render 4...OK
Render 5...OK
Render 6...OK
Render 7...OK
Render 8...OK
Render 9...OK
Render 10...OK
Render 11...OK
Render 12...OK
Render 13...OK
Render 14...OK
Render 15...OK
Render 16...OK
Render 17...OK
Render 18...OK
Render 19...OK
Render 20...OK
Render 21...OK
Render 22...OK
Render 23...OK
Render 24...OK
Render 25...OK
Render 26...OK
Render 27...OK
Render 28...OK
Render 29...OK
Render 30...OK
Render 31...OK
Render 32...OK
Render 33...OK
Render 34...OK
Render 35...OK
Render 36...OK
Render 37...OK
Render 38...OK
Render 39...OK
Render 40...OK
Render 41...OK
Render 42...OK
Render 43...OK
Render 44...OK
Render 45...OK
Render 46...OK
Render 47...OK
Render 48...OK
Render 49...OK
Render 50...OK
Render 51...OK
Render 52...OK
Render 53...OK
Render 54...OK
Render 55...OK
Render 56...OK
Render 57...OK
Render 58...OK
Render 59...OK
Render 60...OK
Render 61...OK
Render 62...OK
Render 63...OK
Render 64...OK
Render 65...OK
Render 66...OK
Render 67...OK
Render 68...OK
Render 69...OK
==DONE==
