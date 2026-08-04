<?php

final class ProbeCache
{
    private const FAILURE_PREFIX = 'failure:';

    /**
     * Runs a probe once per configuration for the test run, caching only a
     * failure. The callback returns its skip reason, or null on success.
     */
    public static function getFailure(string $namespace, array $configuration, callable $probe): ?string
    {
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
                return substr($cached, strlen(self::FAILURE_PREFIX));
            }

            $failure = $probe();
            if (is_string($failure)) {
                rewind($cache);
                ftruncate($cache, 0);
                fwrite($cache, self::FAILURE_PREFIX . $failure);
                fflush($cache);
            }

            return $failure;
        } finally {
            flock($cache, LOCK_UN);
            fclose($cache);
        }
    }
}
