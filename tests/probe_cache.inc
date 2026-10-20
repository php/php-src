<?php

final class ProbeFailureException extends Exception
{
    public function __construct(string|Throwable $failure, ?Throwable $previous = null)
    {
        if ($failure instanceof Throwable) {
            $previous = $failure;
            $failure = $failure->getMessage();
        }

        parent::__construct($failure, 0, $previous);
    }
}

final class ProbeCache
{
    private const FAILURE_PREFIX = 'failure:';

    /**
     * Runs a probe once per configuration during SKIPIF, caching only failures.
     * Outside SKIPIF, the cache is bypassed and wrapped exceptions are rethrown.
     */
    public static function getFailure(string $namespace, array $configuration, callable $probe): mixed
    {
        if (getenv('TEST_PHP_EVALUATING_SKIPIF') !== '1') {
            try {
                return $probe();
            } catch (ProbeFailureException $e) {
                throw $e->getPrevious() ?? $e;
            }
        }

        $directory = getenv('TEST_PHP_SHARED_CACHE_DIR');
        if (!is_string($directory) || !is_dir($directory)) {
            return $probe();
        }

        $cacheFile = $directory
            . DIRECTORY_SEPARATOR
            . 'probe-'
            . hash('sha256', serialize([$namespace, $configuration]));

        $cache = @fopen($cacheFile, 'c+');

        if ($cache === false || !flock($cache, LOCK_EX)) {
            if ($cache !== false) {
                fclose($cache);
            }
            return $probe();
        }

        try {
            $cached = stream_get_contents($cache);
            if (is_string($cached) && str_starts_with($cached, self::FAILURE_PREFIX)) {
                throw new ProbeFailureException(substr($cached, strlen(self::FAILURE_PREFIX)));
            }

            try {
                return $probe();
            } catch (ProbeFailureException $e) {
                rewind($cache);
                ftruncate($cache, 0);
                fwrite($cache, self::FAILURE_PREFIX . $e->getMessage());
                fflush($cache);
                throw $e;
            }
        } finally {
            flock($cache, LOCK_UN);
            fclose($cache);
        }
    }
}
