--TEST--
Benchmark warmup repeats (-T N,M) must not reuse a freed script path
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
include __DIR__ . "/include.inc";

$php = get_cgi_path();
reset_env_vars();

$script = tempnam(sys_get_temp_dir(), "phpbench") . ".php";
file_put_contents($script, "<?php echo 'run\n';");

function run_benchmark(string $php, string $args): string
{
    $out = shell_exec("$php -n $args 2>&1");
    /* The elapsed time is written to stderr; drop it so that any leak report
     * produced at request shutdown remains visible in the comparison. */
    return preg_replace("/\nElapsed time: [0-9.]+ sec\n/", "", $out);
}

/* The script must be executed warmup_repeats + repeats times, each time with
 * a valid script path (previously path_translated aliased script_file, so the
 * buffer was freed after the first request and reused as a dangling pointer).
 * The exact output also catches a leak report at request shutdown. */
$out = run_benchmark($php, "-f " . escapeshellarg($script) . " -T 3,2");
var_dump($out === str_repeat("run\n", 5));

/* The script can also be given as a positional argument, in which case the
 * getopt state has to be reset for every benchmark iteration. */
$out = run_benchmark($php, "-q -T 3,2 " . escapeshellarg($script));
var_dump($out === str_repeat("run\n", 5));

unlink($script);
echo "Done\n";
?>
--EXPECT--
bool(true)
bool(true)
Done
