<?php

namespace FPM;

class LogReader
{
    /**
     * Log debugging.
     *
     * @var bool
     */
    private bool $debug;

    /**
     * Log descriptor.
     *
     * @var string|null
     */
    private ?string $currentSourceName;

    /**
     * Log descriptors.
     *
     * @var LogSource[]
     */
    private array $sources = [];

    /**
     * Log reader constructor.
     *
     * @param bool $debug
     */
    public function __construct(bool $debug = false)
    {
        $this->debug = $debug;
    }

    /**
     * Returns log descriptor source.
     *
     * @return LogSource
     * @throws \Exception
     */
    private function getSource(): LogSource
    {
        if ( ! $this->currentSourceName) {
            throw new \Exception('Log descriptor is not set');
        }

        return $this->sources[$this->currentSourceName];
    }

    /**
     * Set current stream source and create it if it does not exist.
     *
     * @param string   $name   Stream name.
     * @param resource $stream The actual stream.
     */
    public function setStreamSource(string $name, $stream)
    {
        $this->currentSourceName = $name;
        if ( ! isset($this->sources[$name])) {
            $this->sources[$name] = new LogStreamSource($stream);
        }
    }

    /**
     * Set file source as current and create it if it does not exist.
     *
     * @param string $name     Source name.
     * @param string $filePath Source file path.s
     */
    public function setFileSource(string $name, string $filePath)
    {
        $this->currentSourceName = $name;
        if ( ! isset($this->sources[$name])) {
            $this->sources[$name] = new LogFileSource($filePath);
        }
    }

    /**
     * Get a single log line.
     *
     * @param int  $timeoutSeconds      Read timeout in seconds
     * @param int  $timeoutMicroseconds Read timeout in microseconds
     * @param bool $throwOnTimeout      Whether to throw an exception on timeout
     *
     * @return null|string
     * @throws \Exception
     */
    public function getLine(
        int $timeoutSeconds = 3,
        int $timeoutMicroseconds = 0,
        bool $throwOnTimeout = false
    ): ?string {
        $line = $this->getSource()->getLine(
            $timeoutSeconds,
            $timeoutMicroseconds,
            $throwOnTimeout
        );
        $this->trace(is_null($line) ? "LINE - null" : "LINE: $line");

        return $line;
    }

    /**
     * Print separation line.
     */
    public function printSeparator(): void
    {
        echo str_repeat('-', 68) . "\n";
    }

    /**
     * Print all logs.
     */
    public function printLogs(): void
    {
        $hasMultipleDescriptors = count($this->sources) > 1;
        echo "LOGS:\n";
        foreach ($this->sources as $name => $source) {
            if ($hasMultipleDescriptors) {
                echo ">>> source: $name\n";
            }
            $this->printSeparator();
            foreach ($source->getAllLines() as $line) {
                echo $line;
            }
            $this->printSeparator();
        }
    }

    /**
     * Print error and logs.
     *
     * @param string|null $errorMessage Error message to print before the logs.
     *
     * @return false
     */
    public function printError(?string $errorMessage): bool
    {
        if (is_null($errorMessage)) {
            return false;
        }
        echo "ERROR: " . $errorMessage . "\n\n";
        $this->printLogs();
        echo "\n";

        return false;
    }

    /**
     * Read log until matcher matches the log message or there are no more logs.
     *
     * @param callable    $matcher             Callback to identify a match
     * @param string|null $notFoundMessage     Error message if matcher does not succeed.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function readUntil(
        callable $matcher,
        string $notFoundMessage = null,
        bool $checkAllLogs = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        if (is_null($timeoutSeconds) && is_null($timeoutMicroseconds)) {
            $timeoutSeconds      = 3;
            $timeoutMicroseconds = 0;
        } elseif (is_null($timeoutSeconds)) {
            $timeoutSeconds = 0;
        } elseif (is_null($timeoutMicroseconds)) {
            $timeoutMicroseconds = 0;
        }

        $startTime = microtime(true);
        $endTime   = $startTime + $timeoutSeconds + ($timeoutMicroseconds / 1_000_000);
        if ($checkAllLogs) {
            foreach ($this->getSource()->getAllLines() as $line) {
                if ($matcher($line)) {
                    return true;
                }
            }
        }

        do {
            if (microtime(true) > $endTime) {
                return $this->printError($notFoundMessage);
            }
            $line = $this->getLine($timeoutSeconds, $timeoutMicroseconds);
            if ($line === null || microtime(true) > $endTime) {
                return $this->printError($notFoundMessage);
            }
        } while ( ! $matcher($line));

        return true;
    }

    /**
     * Print tracing message - only in debug .
     *
     * @param string $msg Message to print.
     */
    private function trace(string $msg): void
    {
        if ($this->debug) {
            print "LogReader - $msg";
        }
    }
}

class LogTimoutException extends \Exception
{
}

abstract class LogSource
{
    /**
     * Get single line from the source.
     *
     * @param int  $timeoutSeconds      Read timeout in seconds
     * @param int  $timeoutMicroseconds Read timeout in microseconds
     * @param bool $throwOnTimeout      Whether to throw an exception on timeout
     *
     * @return string|null
     * @throws LogTimoutException
     */
    public abstract function getLine(
        int $timeoutSeconds,
        int $timeoutMicroseconds,
        bool $throwOnTimeout = false
    ): ?string;

    /**
     * Get all lines that has been returned by getLine() method.
     *
     * @return string[]
     */
    public abstract function getAllLines(): array;
}

class LogStreamSource extends LogSource
{
    /**
     * @var resource
     */
    private $stream;

    /**
     * @var array
     */
    private array $lines = [];

    public function __construct($stream)
    {
        $this->stream = $stream;
    }

    /**
     * Get single line from the stream.
     *
     * @param int  $timeoutSeconds      Read timeout in seconds
     * @param int  $timeoutMicroseconds Read timeout in microseconds
     * @param bool $throwOnTimeout      Whether to throw an exception on timeout
     *
     * @return string|null
     * @throws LogTimoutException
     */
    public function getLine(
        int $timeoutSeconds,
        int $timeoutMicroseconds,
        bool $throwOnTimeout = false
    ): ?string {
        if (feof($this->stream)) {
            return null;
        }
        $read   = [$this->stream];
        $write  = null;
        $except = null;
        if (stream_select($read, $write, $except, $timeoutSeconds, $timeoutMicroseconds)) {
            $line          = fgets($this->stream);
            $this->lines[] = $line;

            return $line;
        } else {
            if ($throwOnTimeout) {
                throw new LogTimoutException('Timout exceeded when reading line');
            }

            return null;
        }
    }

    /**
     * Get all stream read lines.
     *
     * @return string[]
     */
    public function getAllLines(): array
    {
        return $this->lines;
    }
}

class LogFileSource extends LogSource
{
    /**
     * @var string
     */
    private string $filePath;

    /**
     * @var int
     */
    private int $position;

    /**
     * @var array
     */
    private array $lines = [];

    public function __construct(string $filePath)
    {
        $this->filePath = $filePath;
        $this->position = 0;
    }

    /**
     * Get single line from the file.
     *
     * @param int  $timeoutSeconds      Read timeout in seconds
     * @param int  $timeoutMicroseconds Read timeout in microseconds
     * @param bool $throwOnTimeout      Whether to throw an exception on timeout
     *
     * @return string|null
     * @throws LogTimoutException
     */
    public function getLine(
        int $timeoutSeconds,
        int $timeoutMicroseconds,
        bool $throwOnTimeout = false
    ): ?string {
        $endTime = microtime(true) + $timeoutSeconds + ($timeoutMicroseconds / 1_000_000);
        while ($this->position >= count($this->lines)) {
            if (is_file($this->filePath)) {
                $lines = file($this->filePath);
                if ($lines === false) {
                    return null;
                }
                $this->lines = $lines;
                if ($this->position < count($lines)) {
                    break;
                }
            }
            usleep(50_000);
            if (microtime(true) > $endTime) {
                if ($throwOnTimeout) {
                    throw new LogTimoutException('Timout exceeded when reading line');
                }

                return null;
            }
        }

        return $this->lines[$this->position++];
    }

    /**
     * Get all returned lines from the file.
     *
     * @return string[]
     */
    public function getAllLines(): array
    {
        return array_slice($this->lines, 0, $this->position);
    }
}
