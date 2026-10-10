<?php

require __DIR__ . '/scheduler.inc';

/* A child holding a copy of the pipe on its stdin reports the descriptor's
 * blocking mode each time a line reaches its gate */
function pipe_mode_probe($stream): array
{
    $code = '$g = fopen("php://fd/3", "r"); while (fgets($g) !== false) { echo (int) stream_get_meta_data(STDIN)["blocked"], "\n"; }';
    $proc = proc_open([PHP_BINARY, '-n', '-r', $code], [0 => $stream, 1 => ['pipe', 'w'], 3 => ['pipe', 'r']], $pipes);
    return [$proc, $pipes[3], $pipes[1]];
}

function pipe_mode_ask(array $probe): string
{
    fwrite($probe[1], "?\n");
    return trim(fgets($probe[2]));
}

function pipe_mode_probe_close(array $probe): void
{
    fclose($probe[1]);
    fclose($probe[2]);
    proc_close($probe[0]);
}

/* Reads through a provider, which switches the pipe to non-blocking */
function pipe_mode_read_hooked($stream, int $len, ?Io\OperationQueue $queue = null): string|false
{
    $s = new Scheduler($queue);
    $ret = null;
    Io\Hooks\set_hooks($s);
    $s->spawn(function () use ($stream, $len, &$ret) { $ret = fread($stream, $len); });
    $s->loop();
    Io\Hooks\set_hooks(null);
    return $ret;
}
