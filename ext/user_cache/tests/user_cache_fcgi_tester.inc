<?php

class FcgiTester
{
    private const BEGIN_REQUEST = 1;
    private const END_REQUEST = 3;
    private const PARAMS = 4;
    private const STDIN = 5;
    private const STDOUT = 6;
    private const STDERR = 7;
    private const RESPONDER = 1;

    private string $root;
    private int $port = 0;
    private $process = null;
    private array $pipes = [];

    /* Creates an empty per-process temp root named after $name. */
    public function __construct(string $name)
    {
        $this->root = sys_get_temp_dir() . '/php-user-cache-fcgi-' . $name . '-' . getmypid();
        self::rmRf($this->root);
        mkdir($this->root, 0777, true);
    }

    /* Returns the php-cgi binary path, or null when none is available. */
    public static function binary(): ?string
    {
        $root = dirname(__DIR__, 3);
        foreach ([getenv('TEST_PHP_CGI_EXECUTABLE') ?: null, $root . '/sapi/cgi/php-cgi'] as $candidate) {
            if ($candidate !== null && is_file($candidate) && is_executable($candidate)) {
                return $candidate;
            }
        }

        return null;
    }

    /* Reserves and returns a free loopback TCP port. */
    public static function freePort(): int
    {
        $server = stream_socket_server('tcp://127.0.0.1:0', $errno, $errstr);
        if ($server === false) {
            throw new RuntimeException($errstr);
        }

        $name = stream_socket_get_name($server, false);
        fclose($server);

        return (int) substr(strrchr($name, ':'), 1);
    }

    /* Returns an absolute path below the temp root. */
    public function path(string $relative): string
    {
        return $this->root . '/' . $relative;
    }

    /* Creates a document root holding index.php with the given source and returns its path. */
    public function docRoot(string $name, string $script): string
    {
        $dir = $this->path($name);
        mkdir($dir, 0777, true);
        file_put_contents($dir . '/index.php', $script);

        return $dir;
    }

    /* Writes php.ini from $ini lines, starts php-cgi -b and waits until the given request is served. */
    public function start(array $ini, string $script, string $docRoot, string $serverName, string $query): void
    {
        $binary = self::binary() ?? throw new RuntimeException('CGI SAPI binary not available');
        if (!preg_grep('/^user_cache\.lockfile_path=/', $ini)) {
            $ini[] = 'user_cache.lockfile_path=' . $this->root;
        }
        file_put_contents($this->path('php.ini'), implode("\n", $ini));
        $this->port = self::freePort();

        $this->process = proc_open(
            [$binary, '-c', $this->root, '-b', "127.0.0.1:{$this->port}"],
            [['pipe', 'r'], ['pipe', 'w'], ['pipe', 'w']],
            $this->pipes,
            $this->root
        );
        if (!is_resource($this->process)) {
            throw new RuntimeException('Unable to start php-cgi');
        }
        stream_set_blocking($this->pipes[1], false);
        stream_set_blocking($this->pipes[2], false);

        for ($i = 0; $i < 50; $i++) {
            if (!proc_get_status($this->process)['running']) {
                throw new RuntimeException($this->stderr());
            }

            try {
                $this->request($script, $docRoot, $serverName, $query);
                return;
            } catch (Throwable) {
                usleep(100000);
            }
        }

        throw new RuntimeException($this->stderr() ?: 'php-cgi did not become ready');
    }

    /* Sends one GET request and returns the response body; HTTP_HOST defaults to SERVER_NAME. */
    public function request(string $script, string $docRoot, string $serverName, string $query, ?string $httpHost = null): string
    {
        $fp = @stream_socket_client("tcp://127.0.0.1:{$this->port}", $errno, $errstr, 2);
        if ($fp === false) {
            throw new RuntimeException($errstr);
        }
        stream_set_timeout($fp, 5);

        $params = [
            'SCRIPT_FILENAME' => $script,
            'SCRIPT_NAME' => '/index.php',
            'QUERY_STRING' => $query,
            'REQUEST_METHOD' => 'GET',
            'SERVER_NAME' => $serverName,
            'HTTP_HOST' => $httpHost ?? $serverName,
            'DOCUMENT_ROOT' => $docRoot,
            'REQUEST_URI' => '/index.php' . ($query !== '' ? '?' . $query : ''),
            'SERVER_PROTOCOL' => 'HTTP/1.1',
            'REMOTE_ADDR' => '127.0.0.1',
            'REDIRECT_STATUS' => '1',
        ];

        fwrite($fp, self::record(self::BEGIN_REQUEST, pack('nC6', self::RESPONDER, 0, 0, 0, 0, 0, 0)));
        fwrite($fp, self::record(self::PARAMS, self::params($params)));
        fwrite($fp, self::record(self::PARAMS, ''));
        fwrite($fp, self::record(self::STDIN, ''));

        $stdout = '';
        $stderr = '';
        $ended = false;

        while (!feof($fp)) {
            $header = self::readExact($fp, 8);
            $type = ord($header[1]);
            $contentLength = unpack('n', substr($header, 4, 2))[1];
            $paddingLength = ord($header[6]);
            $content = $contentLength > 0 ? self::readExact($fp, $contentLength) : '';
            if ($paddingLength > 0) {
                self::readExact($fp, $paddingLength);
            }

            if ($type === self::STDOUT) {
                $stdout .= $content;
            } elseif ($type === self::STDERR) {
                $stderr .= $content;
            } elseif ($type === self::END_REQUEST) {
                $ended = true;
                break;
            }
        }

        fclose($fp);

        if (!$ended) {
            throw new RuntimeException('FastCGI connection closed before the end of the request');
        }

        if ($stderr !== '') {
            throw new RuntimeException($stderr);
        }

        $parts = preg_split("/\r?\n\r?\n/", $stdout, 2);

        return trim($parts[1] ?? $stdout);
    }

    /* Throws unless the response body equals $expected, appending php-cgi stderr to the message. */
    public function expect(string $expected, string $script, string $docRoot, string $serverName, string $query, ?string $httpHost = null): void
    {
        $actual = $this->request($script, $docRoot, $serverName, $query, $httpHost);
        if ($actual !== $expected) {
            $stderr = $this->stderr();
            throw new RuntimeException("Expected $expected, got $actual" . ($stderr !== '' ? "\n$stderr" : ''));
        }
    }

    /* Returns what php-cgi has written to stderr so far. */
    public function stderr(): string
    {
        return isset($this->pipes[2]) ? (string) stream_get_contents($this->pipes[2]) : '';
    }

    /* Terminates php-cgi. */
    public function stop(): void
    {
        if (is_resource($this->process)) {
            proc_terminate($this->process);
            proc_close($this->process);
        }
        $this->process = null;
        $this->pipes = [];
    }

    /* Stops php-cgi and removes the temp root and the boundary segments created below it. */
    public function cleanup(): void
    {
        $this->stop();
        self::removeBoundarySegments($this->root);
        self::rmRf($this->root);
    }

    /* Boundary segments outlive their processes; their lock files name them. */
    public static function removeBoundarySegments(string $dir): void
    {
        $locks = array_merge(
            glob($dir . '/.PhpUserCacheBnd.*/*.lock') ?: [],
            glob($dir . '/*/.PhpUserCacheBnd.*/*.lock') ?: []
        );
        foreach ($locks as $lock) {
            $segment = '/dev/shm/PUC.' . basename($lock, '.lock');
            if (is_file($segment)) {
                unlink($segment);
            }
        }
    }

    private static function record(int $type, string $content, int $requestId = 1): string
    {
        $padding = (8 - (strlen($content) % 8)) % 8;

        return chr(1)
            . chr($type)
            . pack('n', $requestId)
            . pack('n', strlen($content))
            . chr($padding)
            . "\0"
            . $content
            . str_repeat("\0", $padding);
    }

    private static function nameValue(string $name, string $value): string
    {
        $nameLen = strlen($name);
        $valueLen = strlen($value);

        return ($nameLen < 128 ? chr($nameLen) : pack('N', $nameLen | 0x80000000))
            . ($valueLen < 128 ? chr($valueLen) : pack('N', $valueLen | 0x80000000))
            . $name
            . $value;
    }

    private static function params(array $params): string
    {
        $body = '';
        foreach ($params as $name => $value) {
            $body .= self::nameValue($name, $value);
        }

        return $body;
    }

    private static function readExact($fp, int $length): string
    {
        $buffer = '';
        while (strlen($buffer) < $length && !feof($fp)) {
            $chunk = fread($fp, $length - strlen($buffer));
            if ($chunk === false) {
                throw new RuntimeException('Failed to read FastCGI response');
            }
            if ($chunk === '') {
                if (stream_get_meta_data($fp)['timed_out']) {
                    throw new RuntimeException('Timed out reading FastCGI response');
                }
                usleep(10000);
                continue;
            }
            $buffer .= $chunk;
        }

        if (strlen($buffer) !== $length) {
            throw new RuntimeException('Truncated FastCGI response');
        }

        return $buffer;
    }

    private static function rmRf(string $path): void
    {
        if (!file_exists($path)) {
            return;
        }
        if (!is_dir($path) || is_link($path)) {
            unlink($path);
            return;
        }
        foreach (scandir($path) as $entry) {
            if ($entry !== '.' && $entry !== '..') {
                self::rmRf($path . DIRECTORY_SEPARATOR . $entry);
            }
        }
        rmdir($path);
    }
}
