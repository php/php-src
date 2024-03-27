<?php

readonly class ProcessResult {
    public function __construct(
        public string $stdout,
        public string $stderr,
    ) {}
}

readonly class UnescapedArg {
    public function __construct(
        public string $arg,
    ) {}
}

function runCommand(array $args, ?string $cwd = null, ?array $envVars = null): ProcessResult {
    $cmd = implode(' ', array_map(function (string|UnescapedArg $v): string {
        return $v instanceof UnescapedArg
            ? $v->arg
            : escapeshellarg($v);
    }, $args));
    $pipes = null;
    $descriptorSpec = [0 => ['pipe', 'r'], 1 => ['pipe', 'w'], 2 => ['pipe', 'w']];
    fwrite(STDOUT, "> $cmd\n");
    $processHandle = proc_open($cmd, $descriptorSpec, $pipes, $cwd ?? getcwd(), $envVars);

    $stdin = $pipes[0];
    $stdout = $pipes[1];
    $stderr = $pipes[2];

    fclose($stdin);

    stream_set_blocking($stdout, false);
    stream_set_blocking($stderr, false);

    $stdoutStr = '';
    $stderrStr = '';
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
                $stdoutStr .= $chunk;
            } elseif ($stream === $stderr) {
                $stderrStr .= $chunk;
            }
        }

        $stdoutEof = $stdoutEof || feof($stdout);
        $stderrEof = $stderrEof || feof($stderr);
    } while(!$stdoutEof || !$stderrEof);

    fclose($stdout);
    fclose($stderr);

    $result = new ProcessResult($stdoutStr, $stderrStr);

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
