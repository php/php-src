#!/usr/bin/env php
<?php declare(strict_types=1);

if (PHP_OS_FAMILY === 'Windows') {
    echo 'Can not work on Windows' . PHP_EOL;
    exit(1);
}

function download(string $uri, string $output): void
{
    passthru("wget $uri -O $output", $exit);
    if ($exit !== 0) {
        passthru("curl -LO $uri -o $output", $exit);
    }
    if ($exit !== 0) {
        throw new Exception("Failed to download '$uri'");
    }
}

function sync(
    string $name,
    string $workspace, string $uri,
    string $subPath = '', callable $fileFilter = null, callable $contentFilter = null,
    string $license = '',
)
{
    $tarName = $name . '-' . basename($uri);
    download($uri, "$workspace/$tarName");

    // clear previous files
    if ($fileFilter) {
        $files = @scandir($workspace);
        if ($files === null) {
            throw new Exception(error_get_last()['message']);
        }
        foreach ($files as $file) {
            $path = "{$workspace}/{$file}";
            if ($fileFilter($path)) {
                unlink($path);
            }
        }
    }

    try {
        $depth = substr_count(trim($subPath, '/'), '/') + 1;
        passthru("tar xvzf $workspace/$tarName -C $workspace --strip-components $depth $subPath", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract $name tarball");
        }
        if ($fileFilter || $contentFilter) {
            foreach (new RecursiveIteratorIterator(new RecursiveDirectoryIterator($workspace)) as $info) {
                $pathName = $info->getPathName();
                if (is_file($pathName)) {
                    if (realpath($pathName) === realpath(__FILE__)) {
                        // workaround for skipping this file
                        continue;
                    } elseif ($fileFilter && !$fileFilter($pathName)) {
                        unlink($pathName);
                    } else if ($contentFilter) {
                        file_put_contents($pathName, $contentFilter(file_get_contents($pathName)));
                    }
                }
            }
        }
        if ($license) {
            download($license, "$workspace/LICENSE");
        }
    } finally {
        @unlink("$workspace/$tarName");
    }
}

// get fiber_asm_file vars from configure.ac
$configureContent = file_get_contents(__DIR__ . '/../../configure.ac');
$fiber_asm_files = [];
preg_match_all('/fiber_asm_file="([^"]+)"/', $configureContent, $fiber_asm_file_expressions);
$fiber_asm_file_expressions = $fiber_asm_file_expressions[1];
foreach ($fiber_asm_file_expressions as $fiber_asm_file_expression) {
    if (preg_match_all('/\${(\w+)}/', $fiber_asm_file_expression, $fiber_asm_file_var_names)) {
        foreach ($fiber_asm_file_var_names[1] as $fiber_asm_file_var_name) {
            preg_match_all("/$fiber_asm_file_var_name=\"([^\"]+)\"/", $configureContent, $fiber_asm_file_vars);
            $fiber_asm_file_vars = $fiber_asm_file_vars[1];
            foreach ($fiber_asm_file_vars as $fiber_asm_file_var) {
                if ($fiber_asm_file_var === 'unknown') {
                    continue;
                }
                $fiber_asm_files[] = str_replace('${' . $fiber_asm_file_var_name . '}', $fiber_asm_file_var, $fiber_asm_file_expression);
            }
        }
    } else {
        $fiber_asm_files[] = $fiber_asm_file_expression;
    }
}

sync(
    name: 'context',
    workspace: __DIR__,
    uri: 'https://github.com/boostorg/context/archive/refs/heads/develop.tar.gz',
    subPath: 'context-develop/src/asm',
    fileFilter: function (string $pathName) use ($fiber_asm_files): bool {
        if (!str_starts_with(basename($pathName), 'make_') && !str_starts_with(basename($pathName), 'jump_')) {
            return false;
        }
        // configure.ac
        foreach ($fiber_asm_files as $fiber_asm_file) {
            if (str_ends_with($pathName, "$fiber_asm_file.S")) {
                return true;
            }
        }
        // for darwin
        if (str_ends_with($pathName, "macho_gas.S") &&
            /* we already have combined file */
            (str_contains($pathName, '_combined_') || !str_contains(file_get_contents($pathName), 'No arch\'s'))) {
            return true;
        }
        // win32/build/Makefile.frag.w32
        if (str_ends_with($pathName, 'ms_pe_masm.asm') ||
            str_ends_with($pathName, 'ms_pe_masm.asm')) {
            return true;
        }
        return false;
    },
    contentFilter: fn(string $content): string => preg_replace([
        '/\b_make_fcontext\b/',
        '/\bmake_fcontext\b/',
        '/\b_jump_fcontext\b/',
        '/\bjump_fcontext\b/',
    ], [
        '_zend_make_fcontext',
        'zend_make_fcontext',
        '_zend_jump_fcontext',
        'zend_jump_fcontext',
    ], $content),
    license: 'https://raw.githubusercontent.com/boostorg/boost/master/LICENSE_1_0.txt'
);

echo "Done.\n";
