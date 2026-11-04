<?php

function has_enough_memory(int|float $bytes): bool
{
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        // Windows-based memory check
        @exec('wmic OS get FreePhysicalMemory', $output);
        if (isset($output[1])) {
            return (((int)trim($output[1])) * 1024) >= $bytes;
        }
    } elseif (PHP_OS == 'FreeBSD') {
        $lines = explode("\n", `sysctl -a`);
        $infos = array();
        foreach ($lines as $line) {
            if (!$line) {
                continue;
            }
            $tmp = explode(":", $line);
            if (count($tmp) < 2) {
                continue;
            }
            $index = strtolower($tmp[0]);
            $value = trim($tmp[1], " ");
            $infos[$index] = $value;
        }
        $freeMemory = ($infos['vm.stats.vm.v_inactive_count'] * $infos['hw.pagesize'])
            + ($infos['vm.stats.vm.v_cache_count'] * $infos['hw.pagesize'])
            + ($infos['vm.stats.vm.v_free_count'] * $infos['hw.pagesize']);
        return $freeMemory >= $bytes;
    } else { // Linux
        // cgroup v1
        $limit = @file_get_contents('/sys/fs/cgroup/memory/memory.limit_in_bytes');
        if ($limit && (int)$limit < $bytes) {
            return false;
        }
        // cgroup v2
        $limit = @file_get_contents('/sys/fs/cgroup/memory.max');
        if ($limit && !str_contains($limit, 'max') && (int)$limit < $bytes) {
            return false;
        }
        // Unix/Linux-based memory check
        $memInfo = @file_get_contents("/proc/meminfo");
        if ($memInfo) {
            preg_match('/MemFree:\s+(\d+) kB/', $memInfo, $matches);
            return ($matches[1] * 1024) >= $bytes; // Convert to bytes
        }
    }
    return false;
}
