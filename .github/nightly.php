<?php

putenv("ASAN_OPTIONS=exitcode=139");
putenv("SYMFONY_DEPRECATIONS_HELPER=max[total]=999");
putenv("PHPSECLIB_ALLOW_JIT=1");

function printMutex(string $result): void {
    flock(STDOUT, LOCK_EX);
    fwrite(STDOUT, $result.PHP_EOL);
    flock(STDOUT, LOCK_UN);
}

function e(string $cmd, string $extra = ''): string {
    exec("bash -c ".escapeshellarg("$cmd 2>&1"), $result, $code);
    $result = implode("\n", $result);
    if ($code) {
        printMutex("An error occurred while executing $cmd (status $code, extra info $extra): $result");
        die(1);
    }
    return $result;
}

$parallel = (int) ($argv[1] ?? 0);
$parallel = $parallel ?: ((int)`nproc`);
$parallel = $parallel ?: 8;

$repos = [];

$repos["phpunit"] = [
    "https://github.com/sebastianbergmann/phpunit.git",
    "main",
    null,
    ["./phpunit"],
    1
];

$repos["infection"] = [
    "https://github.com/infection/infection",
    "master",
    null,
    ["vendor/bin/phpunit"],
    1
];

$repos["wordpress"] = [
    "https://github.com/WordPress/wordpress-develop.git",
    "",
    function (): void {
        $f = file_get_contents('wp-tests-config-sample.php');
        $f = str_replace('youremptytestdbnamehere', 'test', $f);
        $f = str_replace('yourusernamehere', 'root', $f);
        $f = str_replace('yourpasswordhere', 'root', $f);
        file_put_contents('wp-tests-config.php', $f);
    },
    ["vendor/bin/phpunit"],
    1
];

foreach (['amp', 'cache', 'dns', 'file', 'http', 'parallel', 'parser', 'pipeline', 'process', 'serialization', 'socket', 'sync', 'websocket-client', 'websocket-server'] as $repo) {
    $repos["amphp-$repo"] = ["https://github.com/amphp/$repo.git", "", null, ["vendor/bin/phpunit"], 1];
}

$repos["laravel"] = [
    "https://github.com/laravel/framework.git",
    "master",
    function (): void {
        $c = file_get_contents("tests/Filesystem/FilesystemTest.php"); 
        $c = str_replace("public function testSharedGet()", "#[\\PHPUnit\\Framework\\Attributes\\Group('skip')]\n    public function testSharedGet()", $c);
        file_put_contents("tests/Filesystem/FilesystemTest.php", $c);
    },
    ["vendor/bin/phpunit", "--exclude-group", "skip"],
    1
];

foreach (['async', 'cache', 'child-process', 'datagram', 'dns', 'event-loop', 'promise', 'promise-stream', 'promise-timer', 'stream'] as $repo) {
    $repos["reactphp-$repo"] = ["https://github.com/reactphp/$repo.git", "", null, ["vendor/bin/phpunit"], 1];
}

$repos["revolt"] = ["https://github.com/revoltphp/event-loop.git", "", null, ["vendor/bin/phpunit"], 2];

$repos["symfony"] = [
    "https://github.com/symfony/symfony.git",
    "",
    function (): void {
        e("php ./phpunit install");

        // Test causes a heap-buffer-overflow but I cannot reproduce it locally...
        $c = file_get_contents("src/Symfony/Component/HtmlSanitizer/Tests/HtmlSanitizerCustomTest.php");
        $c = str_replace("public function testSanitizeDeepNestedString()", "/** @group skip */\n    public function testSanitizeDeepNestedString()", $c);
        file_put_contents("src/Symfony/Component/HtmlSanitizer/Tests/HtmlSanitizerCustomTest.php", $c);
        // Buggy FFI test in Symfony, see https://github.com/symfony/symfony/issues/47668
        $c = file_get_contents("src/Symfony/Component/VarDumper/Tests/Caster/FFICasterTest.php"); 
        $c = str_replace("*/\n    public function testCastNonTrailingCharPointer()", "* @group skip\n     */\n    public function testCastNonTrailingCharPointer()", $c);
        file_put_contents("src/Symfony/Component/VarDumper/Tests/Caster/FFICasterTest.php", $c);
    },
    function (): iterable {
        $it = new RecursiveDirectoryIterator("src/Symfony");
        /** @var SplFileInfo $file */
        foreach(new RecursiveIteratorIterator($it) as $file) {
            if ($file->getBasename() == 'phpunit.xml.dist') {
                yield [
                    getcwd()."/phpunit",
                    dirname($file->getRealPath()), 
                    "--exclude-group",
                    "tty,benchmark,intl-data,transient",
                    "--exclude-group",
                    "skip"
                ];
            }
        }
    },
    1
];

$finalStatus = 0;
$parentPids = [];

$waitOne = function () use (&$finalStatus, &$parentPids): void {
    $res = pcntl_wait($status);
    if ($res === -1) {
        printMutex("An error occurred while waiting with waitpid!");
        $finalStatus = $finalStatus ?: 1;
        return;
    }
    if (!isset($parentPids[$res])) {
        printMutex("Unknown PID $res returned!");
        $finalStatus = $finalStatus ?: 1;
        return;
    }
    $desc = $parentPids[$res];
    unset($parentPids[$res]);
    if (pcntl_wifexited($status)) {
        $status = pcntl_wexitstatus($status);
        printMutex("Child task $desc exited with status $status");
        if ($status !== 0) {
            $finalStatus = $status;
        }
    } elseif (pcntl_wifstopped($status)) {
        $status = pcntl_wstopsig($status);
        printMutex("Child task $desc stopped by signal $status");
        $finalStatus = 1;
    } elseif (pcntl_wifsignaled($status)) {
        $status = pcntl_wtermsig($status);
        printMutex("Child task $desc terminated by signal $status");
        $finalStatus = 1;
    }
};

$waitAll = function () use ($waitOne, &$parentPids): void {
    while ($parentPids) {
        $waitOne();
    }
};

printMutex("Cloning repos...");

foreach ($repos as $dir => [$repo, $branch, $prepare, $command, $repeat]) {
    $pid = pcntl_fork();
    if ($pid) {
        $parentPids[$pid] = "clone $dir";
        continue;
    }

    chdir(sys_get_temp_dir());
    if ($branch) {
        $branch = "--branch $branch";
    }
    e("git clone $repo $branch --depth 1 $dir");
    
    exit(0);
}

$waitAll();

printMutex("Done cloning repos!");

printMutex("Preparing repos (max $parallel processes)...");
foreach ($repos as $dir => [$repo, $branch, $prepare, $command, $repeat]) {
    chdir(sys_get_temp_dir()."/$dir");
    $rev = e("git rev-parse HEAD", $dir);

    $pid = pcntl_fork();
    if ($pid) {
        $parentPids[$pid] = "prepare $dir ($rev)";
        if (count($parentPids) >= $parallel) {
            $waitOne();
        }
        continue;
    }

    e("composer i --ignore-platform-reqs", $dir);
    if ($prepare) {
        $prepare();
    }

    exit(0);
}
$waitAll();

printMutex("Done preparing repos!");

printMutex("Running tests (max $parallel processes)...");
foreach ($repos as $dir => [$repo, $branch, $prepare, $command, $repeat]) {
    chdir(sys_get_temp_dir()."/$dir");
    $rev = e("git rev-parse HEAD", $dir);

    if ($command instanceof Closure) {
        $commands = iterator_to_array($command());
    } else {
        $commands = [$command];
    }

    foreach ($commands as $idx => $cmd) {
        $cmd = array_merge([
            'php',
            '--repeat',
            $repeat,
            '-f',
            __DIR__.'/jit_check.php',
        ], $cmd);

        $cmdStr = implode(" ", $cmd);

        $pid = pcntl_fork();
        if ($pid) {
            $parentPids[$pid] = "test $dir ($rev): $cmdStr";
            if (count($parentPids) >= $parallel) {
                $waitOne();
            }
            continue;
        }

        $output = sys_get_temp_dir()."/out_{$dir}_$idx.txt";
        
        $p = proc_open($cmd, [
            ["pipe", "r"], 
            ["file", $output, "a"],
            ["file", $output, "a"]
        ], $pipes, sys_get_temp_dir()."/$dir");

        if ($p === false) {
            printMutex("Failure starting $cmdStr");
            exit(1);
        }
        
        $final = 0;
        $status = proc_close($p);
        if ($status !== 0) {
            if ($status > 128) {
                $final = $status;
            }
            printMutex(
                "$dir ($rev): $cmdStr terminated with status $status:".PHP_EOL
                .file_get_contents($output).PHP_EOL
            );
        }

        exit($final);
    }
}

$waitAll();

printMutex("All done!");

die($finalStatus);
