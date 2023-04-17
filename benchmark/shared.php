<?php

class ProcessResult {
    public $stdout;
    public $stderr;
}

function runCommand(array $args, ?string $cwd = null): ProcessResult {
    $cmd = implode(' ', array_map('escapeshellarg', $args));
    $pipes = null;
    $result = new ProcessResult();
    $descriptorSpec = [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']];
    fwrite(STDOUT, "> $cmd\n");
    $processHandle = proc_open($cmd, $descriptorSpec, $pipes, $cwd ?? getcwd(), null);

    $stdin = $pipes[0];
    $stdout = $pipes[1];
    $stderr = $pipes[2];

    fclose($stdin);

    stream_set_blocking($stdout, false);
    stream_set_blocking($stderr, false);

    $stdoutEof = false;
    $stderrEof = false;

    do {
        $read = [$stdout, $stderr];
        $write = null;
        $except = null;

        stream_select($read, $write, $except, 1, 0);

        foreach ($read as $stream) {
            $chunk = fgets($stream);
            if ($stream === $stdout) {
                $result->stdout .= $chunk;
            } elseif ($stream === $stderr) {
                $result->stderr .= $chunk;
            }
        }

        $stdoutEof = $stdoutEof || feof($stdout);
        $stderrEof = $stderrEof || feof($stderr);
    } while(!$stdoutEof || !$stderrEof);

    fclose($stdout);
    fclose($stderr);

    $statusCode = proc_close($processHandle);
    if ($statusCode !== 0) {
        fwrite(STDOUT, $result->stdout);
        fwrite(STDERR, $result->stderr);
        fwrite(STDERR, 'Exited with status code ' . $statusCode . "\n");
        exit($statusCode);
    }

    return $result;
}

function cloneRepo(string $path, string $url) {
    if (is_dir($path)) {
        return;
    }
    $dir = dirname($path);
    $repo = basename($path);
    if (!is_dir($dir)) {
        mkdir($dir, 0755, true);
    }
    runCommand(['git', 'clone', '-q', '--end-of-options', $url, $repo], dirname($path));
}
