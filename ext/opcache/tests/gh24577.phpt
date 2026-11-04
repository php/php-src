--TEST--
Bug GH-24577: CE-cache flag on shared interned class name must not crash with concurrent access
--EXTENSIONS--
opcache
pcntl
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.interned_strings_buffer=8
opcache.file_update_protection=0
--FILE--
<?php
$consumerFile = __DIR__ . '/gh24577_consumer.inc';
$classesFile = __DIR__ . '/gh24577_classes.inc';
$gateFile = __DIR__ . '/gh24577_gate.tmp';
$readyPrefix = __DIR__ . '/gh24577_ready_';
$numClasses = 64;
$numReaders = 6;

@unlink($gateFile);

// 1. Consumer script: holds class-name string literals in SHM interned strings
//    buffer (with refcount = 2 and no CE-cache flag yet).
$consumerCode = "<?php\nfunction gh24577_names(): array {\n    return [\n";
for ($i = 0; $i < $numClasses; $i++) {
    $consumerCode .= sprintf("        'ColdCacheClass%04d',\n", $i);
}
$consumerCode .= "    ];\n}\n";
$consumerCode .= <<<'PHP'
function gh24577_probe(array $names): bool {
    foreach ($names as $class) {
        if (class_exists($class, false)) {
            $obj = new $class();
            if (!($obj instanceof $class)) {
                return false;
            }
        }
    }
    return true;
}
PHP;
file_put_contents($consumerFile, $consumerCode);
touch($consumerFile, time() - 3600);

// 2. Provider script: declares the classes, triggering CE-cache slot publication
//    on the pre-existing shared interned strings when persisted by the writer.
$classesCode = "<?php\n";
for ($i = 0; $i < $numClasses; $i++) {
    $classesCode .= sprintf("class ColdCacheClass%04d {}\n", $i);
}
file_put_contents($classesFile, $classesCode);
touch($classesFile, time() - 3600);

// Step 1: Warm up ONLY the consumer script in OPcache SHM so its class-name
// strings are interned in shared memory with refcount = 2.
$warmup = pcntl_fork();
if ($warmup === -1) {
    die("pcntl_fork failed\n");
} elseif ($warmup === 0) {
    require $consumerFile;
    $status = opcache_get_status();
    exit(isset($status['scripts'][$consumerFile]) ? 0 : 2);
}
pcntl_waitpid($warmup, $wstatus);
if (!pcntl_wifexited($wstatus) || pcntl_wexitstatus($wstatus) !== 0) {
    die("Warmup failed\n");
}

// Step 2: Fork Readers (which load the cached consumer script first, acquiring
// the shared interned strings) and the Writer (which persists $classesFile),
// synchronized by a start barrier.
$children = [];

for ($r = 0; $r < $numReaders; $r++) {
    $pid = pcntl_fork();
    if ($pid === -1) {
        die("pcntl_fork failed\n");
    } elseif ($pid === 0) {
        require $consumerFile;
        $names = gh24577_names();
        file_put_contents($readyPrefix . $r, '1');
        while (!file_exists($gateFile)) {
            usleep(100);
        }
        // Probe shared interned strings while writer is persisting $classesFile
        for ($iter = 0; $iter < 16; $iter++) {
            if (!gh24577_probe($names)) {
                exit(3);
            }
        }
        require $classesFile;
        if (!gh24577_probe($names)) {
            exit(4);
        }
        exit(0);
    }
    $children[] = $pid;
}

$writer = pcntl_fork();
if ($writer === -1) {
    die("pcntl_fork failed\n");
} elseif ($writer === 0) {
    file_put_contents($readyPrefix . 'writer', '1');
    while (!file_exists($gateFile)) {
        usleep(100);
    }
    require $classesFile;
    $status = opcache_get_status();
    exit(isset($status['scripts'][$classesFile]) ? 0 : 5);
}
$children[] = $writer;

// Wait until all readers and the writer are ready at the barrier
$expectedReady = array_map(fn($r) => $readyPrefix . $r, range(0, $numReaders - 1));
$expectedReady[] = $readyPrefix . 'writer';
foreach ($expectedReady as $readyFile) {
    while (!file_exists($readyFile)) {
        usleep(200);
    }
}

// Release barrier so writer persists $classesFile while readers probe $names
file_put_contents($gateFile, '1');

$failed = false;
foreach ($children as $pid) {
    pcntl_waitpid($pid, $status);
    if (!pcntl_wifexited($status) || pcntl_wexitstatus($status) !== 0) {
        $failed = true;
    }
}

foreach ($expectedReady as $readyFile) {
    @unlink($readyFile);
}
@unlink($gateFile);
@unlink($consumerFile);
@unlink($classesFile);

echo $failed ? "Failed\n" : "Done\n";
?>
--CLEAN--
<?php
foreach (glob(__DIR__ . '/gh24577_*') ?: [] as $f) {
    @unlink($f);
}
?>
--EXPECT--
Done
