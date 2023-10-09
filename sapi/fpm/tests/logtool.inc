<?php

namespace FPM;

class LogTool
{
    const P_TIME = '\[\d\d-\w\w\w-\d{4} \d\d:\d\d:\d\d(?:\.\d+)?\]';
    const P_DEBUG = '(?:pid \d+, (?:\w+|\(null\))\(\), line \d+: )?';
    const P_PREFIX = '\[pool unconfined\] child \d+ said into stderr: ';
    const P_PREFIX_STDOUT = '\[pool unconfined\] child \d+ said into stdout: ';
    const FINAL_SUFFIX = ', pipe is closed';

    const DEBUG = 'DEBUG';
    const NOTICE = 'NOTICE';
    const WARNING = 'WARNING';
    const ERROR = 'ERROR';
    const ALERT = 'ALERT';

    /**
     * @var bool
     */
    private bool $debug;

    /**
     * @var LogReader
     */
    private LogReader $logReader;

    /**
     * @var string
     */
    private string $message;

    /**
     * @var string|null
     */
    private ?string $level = null;

    /**
     * @var int
     */
    private int $position;

    /**
     * @var int
     */
    private int $suffixPosition = 0;

    /**
     * @var int
     */
    private int $limit;

    /**
     * @var string|null
     */
    private ?string $pattern;

    /**
     * @var string|null
     */
    private ?string $error = null;

    /**
     * @var bool
     */
    private bool $pipeClosed = false;

    /**
     * Log tool constructor.
     *
     * @param LogReader $logReader
     * @param bool      $debug
     */
    public function __construct(LogReader $logReader, bool $debug = false)
    {
        $this->logReader = $logReader;
        $this->debug     = $debug;
    }

    /**
     * Set expected message for output logging.
     *
     * @param string $message
     * @param int    $limit
     * @param int    $repeat
     */
    public function setExpectedMessage(string $message, int $limit, int $repeat = 0)
    {
        $this->message  = ($repeat > 0) ? str_repeat($message, $repeat) : $message;
        $this->limit    = $limit;
        $this->position = 0;
    }

    /**
     * Set the expected logging level.
     *
     * @param string $level
     *
     * @return string
     */
    public function setExpectedLevel(string $level): string
    {
        return $this->level = $level;
    }

    /**
     * Get the expected logging level.
     *
     * @return string
     */
    public function getExpectedLevel(): string
    {
        return $this->level ?: 'WARNING';
    }

    /**
     * Set whether pipe closed error is shown.
     *
     * @param bool $pipeClosed
     */
    public function setPipeClosed(bool $pipeClosed): void
    {
        $this->pipeClosed = $pipeClosed;
    }

    /**
     * Match the matcher checking the log lines using the callback.
     *
     * @param callable    $matcher             Callback checking whether the log line matches the expected message.
     * @param string|null $notFoundMessage     Error message to show if the message is not found.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    private function match(
        callable $matcher,
        string $notFoundMessage = null,
        bool $checkAllLogs = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        if ($this->getError()) {
            return false;
        }

        if ($this->logReader->readUntil(
            $matcher,
            $notFoundMessage,
            $checkAllLogs,
            $timeoutSeconds,
            $timeoutMicroseconds
        )) {
            $this->popError();

            return true;
        }
        echo $this->popError();

        return false;
    }

    /**
     * @param string|null $line Log line to check against.
     *
     * @return bool
     * @throws \Exception
     */
    public function checkTruncatedMessage(string $line = null): bool
    {
        if ($this->message === null) {
            throw new \LogicException('The message has not been set');
        }

        $matcher = function (string $line) {
            $lineLen = strlen($line);
            if ( ! $this->checkLineLength($line)) {
                return false;
            }
            $this->pattern = '/^PHP message: (.*?)(\.\.\.)?$/';
            if (preg_match($this->pattern, $line, $matches) === 0) {
                return $this->error("Unexpected truncated message: {$line}");
            }

            if ($lineLen === $this->limit - strlen('NOTICE: ') - 1) {
                if ( ! isset($matches[2])) {
                    return $this->error("The truncated line is not ended with '...'");
                }
                if ( ! $this->checkMessage($matches[1])) {
                    return false;
                }
            } else {
                if (isset($matches[2])) {
                    // this is expecting that the expected message does not end with '...'
                    // which should not be an issue for the test purpose.
                    return $this->error("The line is complete and should not end with '...'");
                }
                if ( ! $this->checkMessage($matches[1], -1)) {
                    return false;
                }
            }

            return true;
        };

        if ($line !== null) {
            return $matcher($line);
        } else {
            return $this->match($matcher, 'Truncated message not found');
        }
    }

    /**
     * Check wrapped message.
     *
     * @param bool $terminated Whether to check termination lines.
     * @param bool $decorated  Whether the output is decorated with prefix and suffix.
     * @param bool $isStdErr   Whether the message is written to stderr.
     *
     * @return bool
     * @throws \Exception
     */
    public function checkWrappedMessage(
        bool $terminated = true,
        bool $decorated = true,
        bool $isStdErr = true,
    ): bool {
        if ($this->message === null) {
            throw new \LogicException('The message has not been set');
        }
        if ($decorated) {
            $this->pattern = sprintf(
                '/^(%s %s: %s%s)"([^"]*)"(.*)?$/',
                self::P_TIME,
                $this->getExpectedLevel(),
                self::P_DEBUG,
                $isStdErr ? self::P_PREFIX : self::P_PREFIX_STDOUT
            );
        } else {
            $this->pattern = null;
        }

        $matcher = fn(string $line) => $this->checkLine($line);

        while (strlen($this->message) !== $this->position) {
            if ( ! $this->match($matcher, 'Output message not found')) {
                return false;
            }
        }

        if ($this->suffixPosition > 0) {
            $suffixMatcher = function ($line) use ($isStdErr) {
                $suffixPattern = sprintf(
                    '/^%s %s: %s%s(.*)$/',
                    self::P_TIME,
                    $this->getExpectedLevel(),
                    self::P_DEBUG,
                    $isStdErr ? self::P_PREFIX : self::P_PREFIX_STDOUT
                );
                if (preg_match($suffixPattern, $line, $matches) === 0) {
                    return $this->error("Unexpected line: $line");
                }
                if ($matches[1] !== substr(self::FINAL_SUFFIX, $this->suffixPosition)) {
                    return $this->error(
                        "The suffix has not been finished from position $this->suffixPosition in line: $line"
                    );
                }

                return true;
            };
            if ( ! $this->match($suffixMatcher, 'Suffix message not found')) {
                return false;
            }
            $this->suffixPosition = 0;
        }

        if ($terminated) {
            return $this->expectTerminatorLines();
        }

        return true;
    }

    /**
     * Check workers output line.
     *
     * @param string $line Log output line.
     *
     * @return bool
     */
    private function checkLine(string $line): bool
    {
        $useLine = $this->pattern === null;
        if ($useLine) {
            // plain (not decorated) output
            $out         = rtrim($line);
            $finalSuffix = null;
        } elseif (preg_match($this->pattern, $line, $matches) > 0) {
            $out         = $matches[2];
            $finalSuffix = $matches[3] ?? false;
        } else {
            return $this->error("Unexpected line: $line", $line);
        }

        $rem     = strlen($this->message) - $this->position;
        $lineLen = strlen($line);
        if ( ! $this->checkLineLength($line, $lineLen)) {
            return false;
        }
        if ( ! $this->checkMessage($out, $this->position, $useLine)) {
            return false;
        }
        $outLen         = strlen($out);
        $this->position += $outLen;
        if ($rem > $outLen) { // continuous line
            if ($lineLen !== $this->limit) {
                if ($lineLen + ($rem - $outLen) < $this->limit) {
                    return $this->error("Printed less than the message len");
                }

                return $this->error(
                    "The continuous line length is $lineLen but it should equal to limit $this->limit"
                );
            }

            return true;
        }
        if ($rem !== $outLen) {
            return $this->error("Printed more than the message len");
        }
        if ( ! $this->pipeClosed || $finalSuffix === null) {
            return true;
        }
        if ($finalSuffix === false) {
            return $this->error("No final suffix");
        }
        if (empty($finalSuffix) || ! str_contains(self::FINAL_SUFFIX, $finalSuffix)) {
            return $this->error("The final suffix has to be equal to ', pipe is closed'");
        }
        if (self::FINAL_SUFFIX !== $finalSuffix) {
            $this->suffixPosition = strlen($finalSuffix);
        }

        return true;
    }

    /**
     * Check the message line length - specifically if it's behind the limit.
     *
     * @param string   $line    Log output line.
     * @param int|null $lineLen Line length.
     *
     * @return bool
     */
    private function checkLineLength(string $line, int $lineLen = null): bool
    {
        $lineLen = $lineLen ?: strlen($line);
        if ($lineLen > $this->limit) {
            return $this->error(
                "The line length is $lineLen which is higher than limit $this->limit",
                $line
            );
        }

        return true;
    }

    /**
     * Check whether matched message part matches the expected message.
     *
     * @param string $matchedMessage       The output message or part of it (match).
     * @param int    $expectedMessageStart Message position.
     * @param bool   $isLine               Whether the whole log line is provided as a matched message.
     *
     * @return bool
     */
    private function checkMessage(
        string $matchedMessage,
        int $expectedMessageStart = 0,
        bool $isLine = false
    ): bool {
        if ($expectedMessageStart < 0) {
            $expectedMessage = $this->message;
        } else {
            $expectedMessage = substr($this->message, $expectedMessageStart, strlen($matchedMessage));
        }
        if ($expectedMessage !== $matchedMessage) {
            return $this->error(
                $this->getMatchDebugMessage(
                    sprintf(
                        "The actual string(%d) does not match expected string(%d):\n",
                        strlen($matchedMessage),
                        strlen($expectedMessage)
                    ),
                    expectedMessage: $expectedMessage,
                    actualMessage: "'$matchedMessage'",
                ),
                $isLine ? $matchedMessage : null
            );
        }

        $this->traceMatch(
            "Message matched",
            expectedMessage: $expectedMessage,
            actualMessage: "'$matchedMessage'",
        );

        return true;
    }

    /**
     * Expect log entries for daemon reloading.
     *
     * @param int  $expectedNumberOfSockets
     * @param bool $expectInitialProgressMessage
     * @param bool $expectReloadingMessage
     *
     * @return bool
     * @throws \Exception
     */
    public function expectReloadingLines(
        int $expectedNumberOfSockets,
        bool $expectInitialProgressMessage = true,
        bool $expectReloadingMessage = true
    ): bool {
        if ($expectInitialProgressMessage && ! $this->expectNotice('Reloading in progress ...')) {
            return false;
        }
        if ($expectReloadingMessage && ! $this->expectNotice('reloading: .*')) {
            return false;
        }

        for ($i = 0; $i < $expectedNumberOfSockets; $i++) {
            if ( ! $this->expectNotice('using inherited socket fd=\d+, "[^"]+"')) {
                return false;
            }
        }

        return $this->expectStartingLines();
    }

    /**
     * Expect log entries for reloading logs.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectReloadingLogsLines(): bool
    {
        return (
            $this->expectNotice('error log file re-opened') &&
            $this->expectNotice('access log file re-opened')
        );
    }

    /**
     * Expect starting lines when FPM starts.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectStartingLines(): bool
    {
        if ($this->getError()) {
            return false;
        }

        return (
            $this->expectNotice('fpm is running, pid \d+') &&
            $this->expectNotice('ready to handle connections')
        );
    }

    /**
     * Expect termination lines when FPM terminates.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectTerminatorLines(): bool
    {
        if ($this->getError()) {
            return false;
        }

        return (
            $this->expectNotice('Terminating ...') &&
            $this->expectNotice('exiting, bye-bye!')
        );
    }

    /**
     * Get log entry matcher.
     *
     * @param string $type            Entry type like NOTICE, WARNING, DEBUG and so on.
     * @param string $expectedMessage Message to search for
     * @param string $pool            Pool that is used and prefixes the message.
     * @param string $ignoreErrorFor  Ignore error for supplied string in the message.
     *
     * @return callable
     */
    private function getEntryMatcher(
        string $type,
        string $expectedMessage,
        ?string $pool,
        string $ignoreErrorFor
    ): callable {
        if ($pool !== null) {
            $expectedMessage = '\[pool ' . $pool . '\] ' . $expectedMessage;
        }
        $this->trace("Matching EXPECTED: $expectedMessage");

        // Allow expected message to contain %s and %s for any string or number as in run-tests.php
        $expectRe = str_replace('%s', '[^\r\n]+', $expectedMessage);
        $expectRe = str_replace('%d', '\d+', $expectRe);

        $pattern = sprintf('/^(?:%s )?%s: %s(%s)$/', self::P_TIME, $type, self::P_DEBUG, $expectRe);
        $this->trace("PATTERN: $pattern");

        return function ($line) use ($expectedMessage, $pattern, $type, $ignoreErrorFor) {
            $line = rtrim($line);
            if (preg_match($pattern, $line, $matches) === 0) {
                if ($this->getError()) { // quick bail out to save some CPU
                    return false;
                }

                // get actual message
                $types        = implode('|', [self::NOTICE, self::WARNING, self::ERROR, self::ALERT]);
                $errorPattern = sprintf('/^(?:%s )?(%s): %s(.*)$/', self::P_TIME, $types, self::P_DEBUG);
                if (preg_match($errorPattern, $line, $matches) === 0) {
                    $actualMessage = null;
                } else {
                    $expectedMessage = $type . ' - ' . $expectedMessage;
                    $actualMessage   = $matches[1] . ' - ' . $matches[2];
                }

                return $this->error(
                    $this->getMatchDebugMessage(
                        'Most likely invalid match for entry',
                        $pattern,
                        $line,
                        $expectedMessage,
                        $actualMessage
                    ),
                    $line,
                    $ignoreErrorFor
                );
            }
            $this->trace("Matched ACTUAL: " . $matches[1]);

            return true;
        };
    }

    /**
     * Read all log entries until timeout.
     *
     * @param string      $type            Entry type like NOTICE, WARNING, DEBUG and so on.
     * @param string      $expectedMessage Message to search for
     * @param string|null $pool            Pool that is used and prefixes the message.
     * @param string      $ignoreErrorFor  Ignore error for supplied string in the message.
     *
     * @return bool
     * @throws \Exception
     */
    public function readAllEntries(
        string $type,
        string $expectedMessage,
        string $pool = null,
        string $ignoreErrorFor = self::DEBUG
    ): bool {
        if ($this->getError()) {
            return false;
        }

        $matcher = $this->getEntryMatcher($type, $expectedMessage, $pool, $ignoreErrorFor);

        while ($this->logReader->readUntil($matcher)) {
            $this->popError();
        }
        $this->popError();

        return true;
    }

    /**
     * Expect log entry.
     *
     * @param string      $type                Entry type like NOTICE, WARNING, DEBUG and so on.
     * @param string      $expectedMessage     Message to search for
     * @param string|null $pool                Pool that is used and prefixes the message.
     * @param string      $ignoreErrorFor      Ignore error for supplied string in the message.
     * @param bool        $checkAllLogs        Whether to also check past logs.
     * @param bool        $invert              Whether the log entry is not expected rather than expected.
     * @param int|null    $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null    $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectEntry(
        string $type,
        string $expectedMessage,
        string $pool = null,
        string $ignoreErrorFor = self::DEBUG,
        bool $checkAllLogs = false,
        bool $invert = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null
    ): bool {
        if ($this->getError()) {
            return false;
        }

        $matchResult = $this->match(
            $this->getEntryMatcher($type, $expectedMessage, $pool, $ignoreErrorFor),
            $invert ? null : "The $type does not match expected message",
            $checkAllLogs,
            $timeoutSeconds,
            $timeoutMicroseconds
        );

        if ($matchResult && $invert) {
            return $this->error("The $type matches unexpected message");
        }

        return $matchResult;
    }

    /**
     * Expect debug log entry.
     *
     * @param string      $expectedMessage
     * @param string|null $pool
     *
     * @return bool
     * @throws \Exception
     */
    public function expectDebug(string $expectedMessage, string $pool = null): bool
    {
        return $this->expectEntry(self::DEBUG, $expectedMessage, $pool, self::ERROR);
    }

    /**
     * Expect notice log entry.
     *
     * @param string      $expectedMessage
     * @param string|null $pool
     *
     * @return bool
     * @throws \Exception
     */
    public function expectNotice(string $expectedMessage, string $pool = null): bool
    {
        return $this->expectEntry(self::NOTICE, $expectedMessage, $pool);
    }

    /**
     * Expect warning log entry.
     *
     * @param string      $expectedMessage
     * @param string|null $pool
     *
     * @return bool
     * @throws \Exception
     */
    public function expectWarning(string $expectedMessage, string $pool = null): bool
    {
        return $this->expectEntry(self::WARNING, $expectedMessage, $pool);
    }

    /**
     * Expect error log entry.
     *
     * @param string      $expectedMessage
     * @param string|null $pool
     *
     * @return bool
     * @throws \Exception
     */
    public function expectError(string $expectedMessage, string $pool = null): bool
    {
        return $this->expectEntry(self::ERROR, $expectedMessage, $pool);
    }

    /**
     * Expect alert log entry.
     *
     * @param string      $expectedMessage
     * @param string|null $pool
     *
     * @return bool
     * @throws \Exception
     */
    public function expectAlert(string $expectedMessage, string $pool = null): bool
    {
        return $this->expectEntry(self::ALERT, $expectedMessage, $pool);
    }

    /**
     * Expect pattern in the log line.
     *
     * @param string   $pattern             Pattern to use.
     * @param bool     $invert              Whether to expect pattern not to match.
     * @param bool     $checkAllLogs        Whether to also check past logs.
     * @param int|null $timeoutSeconds      Timeout in seconds for reading of all messages.
     * @param int|null $timeoutMicroseconds Additional timeout in microseconds for reading of all messages.
     *
     * @return bool
     * @throws \Exception
     */
    public function expectPattern(
        string $pattern,
        bool $invert = false,
        bool $checkAllLogs = false,
        int $timeoutSeconds = null,
        int $timeoutMicroseconds = null,
    ): bool {
        $matchResult = $this->match(
            function ($line) use ($pattern) {
                if (preg_match($pattern, $line) === 1) {
                    $this->traceMatch("Pattern expectation", $pattern, $line);

                    return true;
                }

                return false;
            },
            $invert ? null : 'The search pattern not found',
            $checkAllLogs,
            $timeoutSeconds,
            $timeoutMicroseconds
        );

        if ($invert && $matchResult) {
            return $this->logReader->printError('The search pattern found - PATTERN: ' . $pattern);
        }

        return $matchResult;
    }

    /**
     * Get match debug message.
     *
     * @param string      $title
     * @param string|null $pattern
     * @param string|null $line
     * @param string|null $expectedMessage
     * @param string|null $actualMessage
     *
     * @return string
     */
    private function getMatchDebugMessage(
        string $title,
        string $pattern = null,
        string $line = null,
        string $expectedMessage = null,
        string $actualMessage = null
    ): string {
        $msg = "$title:\n";
        if ($pattern !== null) {
            $msg .= "- PATTERN: $pattern\n";
        }
        if ($line !== null) {
            $msg .= "- LINE: $line\n";
        }
        if ($expectedMessage !== null) {
            $msg .= "- EXPECTED: $expectedMessage\n";
        }
        if ($actualMessage !== null) {
            $msg .= "- ACTUAL: $actualMessage\n";
        }

        return $msg;
    }

    /**
     * Print tracing of the match.
     *
     * @param string      $title
     * @param string|null $pattern
     * @param string|null $line
     * @param string|null $expectedMessage
     * @param string|null $actualMessage
     */
    private function traceMatch(
        string $title,
        string $pattern = null,
        string $line = null,
        string $expectedMessage = null,
        string $actualMessage = null
    ): void {
        if ($this->debug) {
            echo "LogTool - " . $this->getMatchDebugMessage($title, $pattern, $line, $expectedMessage, $actualMessage);
        }
    }

    /**
     * Print tracing message - only in debug .
     *
     * @param string $msg Message to print.
     */
    private function trace(string $msg): void
    {
        if ($this->debug) {
            print "LogTool - $msg\n";
        }
    }


    /**
     * Save error message if the line does not contain ignored string.
     *
     * @param string      $msg
     * @param string|null $line
     * @param string      $ignoreFor
     *
     * @return false
     */
    private function error(string $msg, string $line = null, string $ignoreFor = self::DEBUG): bool
    {
        if ($this->error === null && ($line === null || ! str_contains($line, $ignoreFor))) {
            $this->trace("Setting error: $msg");
            $this->error = $msg;
        }

        return false;
    }

    /**
     * Get saved error.
     *
     * @return string|null
     */
    public function getError(): ?string
    {
        return $this->error;
    }

    /**
     * Get saved error and clear it.
     *
     * @return string|null
     */
    public function popError(): ?string
    {
        $error       = $this->error;
        $this->error = null;
        if ($error !== null) {
            $this->trace("Clearing error: $error");
        }

        return $error;
    }
}
