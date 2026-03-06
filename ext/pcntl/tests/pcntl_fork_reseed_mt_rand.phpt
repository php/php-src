--TEST--
pcntl_fork() reseeds MT rand in child processes (GH-21351)
--EXTENSIONS--
pcntl
posix
--FILE--
<?php
/*
 * When MT rand is seeded before fork(), child processes must not inherit
 * the parent's MT state verbatim — each child should get a fresh seed
 * so that mt_rand() / array_rand() / shuffle() produce different sequences.
 */

// Ensure MT rand is seeded before forking
mt_srand(42);
mt_rand();

$tmpfile = tempnam(sys_get_temp_dir(), 'pcntl_mt_');

$children = 5;
for ($i = 0; $i < $children; $i++) {
    $pid = pcntl_fork();
    if ($pid == -1) {
        die("fork failed");
    } else if ($pid == 0) {
        // Child: generate a value and write it to the shared temp file.
        // If reseeding works, children should not all produce the same value.
        $val = mt_rand(0, PHP_INT_MAX);
        file_put_contents($tmpfile, $val . "\n", FILE_APPEND | LOCK_EX);
        exit(0);
    }
}

// Parent: wait for all children
while (pcntl_wait($status) > 0);

$lines = array_filter(array_map('trim', file($tmpfile)));
$unique = array_unique($lines);

// With 5 children and a 31-bit range, getting all identical values
// from truly independent seeds is astronomically unlikely.
if (count($unique) > 1) {
    echo "PASS: children produced different mt_rand values after fork\n";
} else {
    echo "FAIL: all children produced the same mt_rand value: " . $lines[0] . "\n";
}

@unlink($tmpfile);
?>
--EXPECT--
PASS: children produced different mt_rand values after fork
