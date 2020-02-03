<?php

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});

$rootDir = __DIR__ . '/../..';
$it = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator($rootDir),
    RecursiveIteratorIterator::LEAVES_ONLY
);

$excludes = [
    // Bundled libraries / files.
    'ext/bcmath/libbcmath/',
    'ext/date/lib/',
    'ext/fileinfo/data_file.c',
    'ext/fileinfo/libmagic/',
    'ext/gd/libgd/',
    'ext/hash/sha3/',
    'ext/hash/hash_whirlpool.c',
    'ext/hash/php_hash_whirlpool_tables.h',
    'ext/mbstring/libmbfl/',
    'ext/mbstring/unicode_data.h',
    'ext/opcache/jit/dynasm',
    'ext/opcache/jit/libudis86',
    'ext/opcache/jit/vtune',
    'ext/pcre/pcre2lib/',
    'ext/standard/html_tables/html_table_gen.php',
    'ext/xmlrpc/libxmlrpc/',
    'sapi/cli/php_http_parser.c',
    'sapi/cli/php_http_parser.h',
    'sapi/litespeed/',
    // Not a PHP file.
    'ext/zlib/tests/data.inc',
    // Flexible HEREDOC/NOWDOC tests are likely whitespace sensitive.
    // TODO: Properly classify them.
    'Zend/tests/flexible-',
];

$psr12Excludes = [
    'ext/tokenizer/tests',
    'ext/standard/tests/general_functions',
    'ext/standard/tests/strings',
    '.stub.php',
];

// Apply basic formatting

foreach ($it as $file) {
    if (!$file->isFile()) {
        continue;
    }

    $path = $file->getPathName();
    foreach ($excludes as $exclude) {
        if (strpos($path, $exclude) !== false) {
            continue 2;
        }
    }

    $lang = getLanguageFromExtension($file->getExtension());
    if ($lang === null) {
        continue;
    }

    $origCode = $code = file_get_contents($path);

    if ($lang === 'c') {
        //$code = stripTrailingWhitespace($code);
        // TODO: Avoid this for now.
        // $code = reindentToTabs($code);
    } else if ($lang === 'php') {
        $code = stripTrailingWhitespace($code);
        $code = reindentToSpaces($code);

        $formatPsr12 = true;
        foreach ($psr12Excludes as $exclude) {
            if (strpos($path, $exclude) !== false) {
                $formatPsr12 = false;
                break;
            }
        }

        if ($formatPsr12) {
            $code = applyPsr12($code);
        }
    } else if ($lang === 'phpt') {
        // TODO: Don't reformat .phpt on PHP-7.4.
        $code = transformTestCode($code, function(string $code) use ($path, $psr12Excludes) {
            $code = stripTrailingWhitespace($code);
            $code = reindentToSpaces($code);

            foreach ($psr12Excludes as $exclude) {
                if (strpos($path, $exclude) !== false) {
                    return $code;
                }
            }

            $code = applyPsr12($code) . "\n?>\n";

            return $code;
        });
    }

    if ($origCode !== $code) {
        file_put_contents($path, $code);
    }
}

function stripTrailingWhitespace(string $code): string {
    return preg_replace('/\h+$/m', '', $code);
}

function reindentToTabs(string $code): string {
    return preg_replace_callback('/^ +/m', function(array $matches) {
        $tabSize = 4;
        $spaces = strlen($matches[0]);
        $tabs = intdiv($spaces, $tabSize);
        $spaces -= $tabs * $tabSize;
        return str_repeat("\t", $tabs) . str_repeat(" ", $spaces);
    }, $code);
}

function reindentToSpaces(string $code): string {
    return preg_replace_callback('/^[ \t]+/m', function(array $matches) {
        $tabSize = 4;
        $indent = 0;
        foreach (str_split($matches[0]) as $char) {
            if ($char === ' ') {
                $indent++;
            } else {
                $partialIndent = $indent % $tabSize;
                if ($partialIndent === 0) {
                    $indent += $tabSize;
                } else {
                    $indent += $tabSize - $partialIndent;
                }
            }
        }
        return str_repeat(" ", $indent);
    }, $code);
}

function transformTestCode(string $code, callable $transformer): string {
    // Don't transform whitespace-sensitive tests.
    if (strpos($code, '--WHITESPACE_SENSITIVE--') !== false) {
        return $code;
    }

    return preg_replace_callback(
        '/(--FILE--)(.+?)(--[A-Z_]+--|={2,}DONE={2,})/s',
        function(array $matches) use($transformer) {
            return $matches[1] . $transformer($matches[2]) . $matches[3];
        },
        $code
    );
}

function getLanguageFromExtension(string $ext): ?string {
    switch ($ext) {
    case 'c':
    case 'h':
    case 'cpp':
    case 'y':
    case 'l':
    case 're':
        return 'c';
    case 'php':
    // TODO: Reformat .inc files.
    //case 'inc':
        return 'php';
    case 'phpt':
        return 'phpt';
    default:
        return null;
    }
}

function applyPsr12(string $code): string {
    $phpcbfPath = getPhpCodeBeautifierPath();
    $cacheFilePath = __DIR__ . "/formatting-cache.php";

    file_put_contents($cacheFilePath, $code);

    passthru("php $phpcbfPath --standard=PSR12 $cacheFilePath", $exit);

    if ($exit <= 1) {
        $code = file_get_contents($cacheFilePath);
    }

    if (file_exists($cacheFilePath)) {
        unlink($cacheFilePath);
    }

    return $code;
}

function getPhpCodeBeautifierPath(): string {
    $version = "3.5.4";
    $phpCodeSnifferDir = __DIR__ . "/PHP-CodeSniffer-$version";
    if (!is_dir($phpCodeSnifferDir)) {
        $cwd = getcwd();
        chdir(__DIR__);

        passthru("wget https://github.com/squizlabs/PHP_CodeSniffer/archive/$version.tar.gz", $exit);
        if ($exit !== 0) {
            passthru("curl -LO https://github.com/squizlabs/PHP_CodeSniffer/archive/$version.tar.gz", $exit);
        }
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP CodeSniffer tarball");
        }
        if (!mkdir($phpCodeSnifferDir)) {
            throw new Exception("Failed to create directory $phpCodeSnifferDir");
        }
        passthru("tar xvzf $version.tar.gz -C PHP-CodeSniffer-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract PHP-CodeSniffer tarball");
        }
        unlink(__DIR__ . "/$version.tar.gz");
        chdir($cwd);
    }

    return $phpCodeSnifferDir . "/bin/phpcbf";
}
