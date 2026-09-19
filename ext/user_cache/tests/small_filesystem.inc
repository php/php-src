<?php

/* Locate a writable directory on a filesystem too small to back a segment,
 * and pick a user_cache.shm_size that exceeds it. Returns [directory, size]
 * or null. TEST_PHP_USER_CACHE_SMALL_FS overrides the default probe of
 * /dev/shm (64 MiB in the default Docker/Kubernetes configuration). */
function user_cache_small_filesystem(): ?array
{
    $candidates = [];
    $override = getenv('TEST_PHP_USER_CACHE_SMALL_FS');
    if ($override !== false && $override !== '') {
        $candidates[] = $override;
    }
    if (PHP_OS_FAMILY === 'Linux') {
        $candidates[] = '/dev/shm';
    }

    foreach ($candidates as $dir) {
        if (!is_dir($dir) || !is_writable($dir)) {
            continue;
        }

        $free = @disk_free_space($dir);
        if ($free === false) {
            continue;
        }

        /* Leave headroom over the free space; stay well below the INI clamp
         * (slightly under 16384M) so the size is applied as given. */
        $sizeMiB = (int) ceil($free / (1024 * 1024)) + 64;
        if ($sizeMiB > 3584) {
            continue;
        }

        return [rtrim($dir, '/'), $sizeMiB . 'M'];
    }

    return null;
}
