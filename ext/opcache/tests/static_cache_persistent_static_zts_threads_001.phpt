--TEST--
OPcache PinnedStatic class, property, and method state handles cross-thread writes on ZTS builds
--EXTENSIONS--
opcache
--SKIPIF--
<?php
if (!PHP_ZTS) {
    die('skip ZTS build required');
}

if (!function_exists('proc_open')) {
    die('skip proc_open required');
}

$root = realpath(__DIR__ . '/../../..');
if ($root === false) {
    die('skip source root not found');
}

$buildRoot = null;
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
if ($php) {
    $php = realpath($php);
    if ($php !== false) {
        if (PHP_OS_FAMILY === 'Windows') {
            $candidate = dirname($php);
            if (glob($candidate . '/php*embed.lib')) {
                $buildRoot = $candidate;
            }
        } else {
            $candidate = dirname($php, 3);
            if (file_exists($candidate . '/.libs/libphp.a') || file_exists($candidate . '/libs/libphp.a')) {
                $buildRoot = $candidate;
            }
        }
    }
}

if ($buildRoot === null) {
    if (PHP_OS_FAMILY === 'Windows') {
        if (glob($root . '/php*embed.lib')) {
            $buildRoot = $root;
        }
    } elseif (file_exists($root . '/.libs/libphp.a') || file_exists($root . '/libs/libphp.a')) {
        $buildRoot = $root;
    }
}

if ($buildRoot === null) {
    die(PHP_OS_FAMILY === 'Windows' ? 'skip embed library not built' : 'skip libphp.a not built');
}
?>
--FILE--
<?php

const MAX_COMMAND_OUTPUT_BYTES = 16384;

function readCommandOutput(string $path): string
{
    $handle = @fopen($path, 'rb');
    if ($handle === false) {
        return '';
    }

    $contents = stream_get_contents($handle, MAX_COMMAND_OUTPUT_BYTES + 1);
    fclose($handle);

    if ($contents === false) {
        return '';
    }

    if (strlen($contents) > MAX_COMMAND_OUTPUT_BYTES) {
        return substr($contents, 0, MAX_COMMAND_OUTPUT_BYTES) . "\n...[output truncated]\n";
    }

    return $contents;
}

function runCommand(array $command): array
{
    $stdoutFile = tempnam(sys_get_temp_dir(), 'opcache-zts-out-');
    $stderrFile = tempnam(sys_get_temp_dir(), 'opcache-zts-err-');
    if ($stdoutFile === false || $stderrFile === false) {
        throw new RuntimeException('failed to create temporary files');
    }

    $descriptorSpec = [
        0 => ['pipe', 'r'],
        1 => ['file', $stdoutFile, 'w'],
        2 => ['file', $stderrFile, 'w'],
    ];

    $process = proc_open($command, $descriptorSpec, $pipes);
    if (!is_resource($process)) {
        @unlink($stdoutFile);
        @unlink($stderrFile);
        throw new RuntimeException('proc_open failed');
    }

    fclose($pipes[0]);
    $status = proc_close($process);

    $stdout = readCommandOutput($stdoutFile);
    $stderr = readCommandOutput($stderrFile);
    @unlink($stdoutFile);
    @unlink($stderrFile);

    return [$status, $stdout, $stderr];
}

function parseBuildFlags(string $flags): array
{
    $tokens = preg_split('/\s+/', trim($flags));
    if ($tokens === false) {
        return [];
    }

    return array_values(array_filter($tokens, static function (string $token): bool {
        return $token !== '' && !str_contains($token, '$(');
    }));
}

function resolveBuildFlags(string $buildRoot, array $variables): array
{
    $makefile = @file_get_contents($buildRoot . '/Makefile');
    $flags = [];

    if ($makefile === false) {
        return $flags;
    }

    foreach ($variables as $variable) {
        if (preg_match('/^' . preg_quote($variable, '/') . '\s*=\s*(.+)$/m', $makefile, $matches)) {
            $flags = array_merge($flags, parseBuildFlags($matches[1]));
        }
    }

    return $flags;
}

function resolveBuildCommand(string $buildRoot, string $variable, array $fallback): array
{
    $value = getenv($variable);
    if (is_string($value) && $value !== '') {
        $tokens = parseBuildFlags($value);
        if ($tokens !== []) {
            return $tokens;
        }
    }

    $tokens = resolveBuildFlags($buildRoot, [$variable]);
	return $tokens !== [] ? $tokens : $fallback;
}

function resolveWindowsBuildFlags(): array
{
    return [
        '/MD',
        '/DZEND_ENABLE_STATIC_TSRMLS_CACHE=1',
        '/D_WINDOWS',
        '/DWINDOWS=1',
        '/DZEND_WIN32=1',
        '/DPHP_WIN32=1',
        '/DWIN32',
        '/D_MBCS',
        '/D_USE_MATH_DEFINES',
        '/DZTS=1',
        '/DZEND_DEBUG=0',
        '/DNDebug',
        '/DNDEBUG',
    ];
}

function resolveBuildRoot(string $root): string
{
    $buildRoot = null;
    $php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
    if ($php) {
        $php = realpath($php);
        if ($php !== false) {
            if (PHP_OS_FAMILY === 'Windows') {
                $candidate = dirname($php);
                if (glob($candidate . '/php*embed.lib')) {
                    $buildRoot = $candidate;
                }
            } else {
                $candidate = dirname($php, 3);
                if (file_exists($candidate . '/.libs/libphp.a') || file_exists($candidate . '/libs/libphp.a')) {
                    $buildRoot = $candidate;
                }
            }
        }
    }

    if ($buildRoot === null && PHP_OS_FAMILY !== 'Windows' && (file_exists($root . '/.libs/libphp.a') || file_exists($root . '/libs/libphp.a'))) {
        $buildRoot = $root;
    }

    if ($buildRoot === null) {
        throw new RuntimeException('Failed to resolve build root');
    }

    return $buildRoot;
}

function resolveExtraLibs(string $buildRoot): array
{
    if (PHP_OS_FAMILY === 'Windows') {
        return [];
    }

    $makefile = @file_get_contents($buildRoot . '/Makefile');
    if ($makefile !== false && preg_match('/^EXTRA_LIBS\s*=\s*(.+)$/m', $makefile, $matches)) {
        $libs = preg_split('/\s+/', trim($matches[1]));
        if ($libs !== false && $libs !== ['']) {
            return $libs;
        }
    }

    return [
        '-lm',
        '-lpthread',
        '-ldl',
        '-lresolv',
        '-lutil',
    ];
}

function resolveWindowsEmbedLib(string $buildRoot): string
{
    $candidates = glob($buildRoot . '/php*embed.lib');
    if ($candidates !== false && $candidates !== []) {
        return $candidates[0];
    }

    throw new RuntimeException('Failed to resolve Windows embed library');
}

$root = realpath(__DIR__ . '/../../..');
if ($root === false) {
    throw new RuntimeException('Failed to resolve source root');
}

$buildRoot = resolveBuildRoot($root);
if (PHP_OS_FAMILY === 'Windows') {
    putenv('PATH=' . $buildRoot . PATH_SEPARATOR . (getenv('PATH') ?: ''));
}
$source = __DIR__ . '/helpers/pinned_static_zts_threads_001.c';
$binary = __DIR__ . '/helpers/pinned_static_zts_threads_001' . (PHP_OS_FAMILY === 'Windows' ? '.exe' : '.out');
$scenario = __DIR__ . '/helpers/pinned_static_zts_threads_001.inc';
$compiler = resolveBuildCommand($buildRoot, 'CC', PHP_OS_FAMILY === 'Windows' ? ['cl'] : ['gcc']);
$buildFlags = PHP_OS_FAMILY === 'Windows' ? resolveWindowsBuildFlags() : resolveBuildFlags($buildRoot, ['CPPFLAGS', 'CFLAGS_CLEAN']);
$extraLibs = resolveExtraLibs($buildRoot);

if (PHP_OS_FAMILY === 'Windows') {
    $compileCommand = [
		...$compiler,
        '/nologo',
        ...$buildFlags,
        '/I' . $buildRoot,
        '/I' . $buildRoot . '/main',
        '/I' . $buildRoot . '/Zend',
        '/I' . $buildRoot . '/TSRM',
        '/I' . $buildRoot . '/sapi/embed',
        '/I' . $root,
        '/I' . $root . '/main',
        '/I' . $root . '/Zend',
        '/I' . $root . '/TSRM',
        '/I' . $root . '/sapi/embed',
        '/Fe:' . $binary,
        $source,
        '/link',
        resolveWindowsEmbedLib($buildRoot),
    ];
} else {
    $compileCommand = [
		...$compiler,
        ...$buildFlags,
        '-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1',
        '-DHAVE_CONFIG_H',
        '-I' . $buildRoot,
        '-I' . $buildRoot . '/main',
        '-I' . $buildRoot . '/Zend',
        '-I' . $buildRoot . '/TSRM',
        '-I' . $buildRoot . '/sapi/embed',
        '-I' . $root,
        '-I' . $root . '/main',
        '-I' . $root . '/Zend',
        '-I' . $root . '/TSRM',
        '-I' . $root . '/sapi/embed',
        '-o',
        $binary,
        $source,
        $root . '/sapi/embed/php_embed.c',
        file_exists($buildRoot . '/.libs/libphp.a') ? $buildRoot . '/.libs/libphp.a' : $buildRoot . '/libs/libphp.a',
    ];
    $compileCommand = array_merge($compileCommand, $extraLibs);
}

[$status, $stdout, $stderr] = runCommand($compileCommand);
if ($status !== 0) {
    echo $stdout, $stderr;
    exit(1);
}

@chmod($binary, 0755);
[$status, $stdout, $stderr] = runCommand([$binary, $scenario]);
echo $stdout;
if ($status !== 0) {
    echo $stderr;
    exit(1);
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/helpers/pinned_static_zts_threads_001.out');
@unlink(__DIR__ . '/helpers/pinned_static_zts_threads_001.exe');
@unlink(__DIR__ . '/helpers/pinned_static_zts_threads_001.inc.ready');
@unlink(__DIR__ . '/helpers/pinned_static_zts_threads_001.inc.done');
?>
--EXPECT--
ok
