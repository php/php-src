<?php

set_error_handler(function($_, $msg) {
    throw new Exception($msg);
});

if ($argc > 1) {
    $dir = $argv[1];
} else {
    $dir = __DIR__ . '/../..';
}
if (!is_dir($dir)) {
    echo "Directory $dir does not exist.\n";
    exit(1);
}

$it = new RecursiveIteratorIterator(
    new RecursiveDirectoryIterator($dir),
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
    'sapi/cli/php_http_parser.c',
    'sapi/cli/php_http_parser.h',
    'sapi/litespeed/',
    // Not a PHP file.
    'ext/zlib/tests/data.inc',
    // Flexible HEREDOC/NOWDOC tests are likely whitespace sensitive.
    // TODO: Properly classify them.
    'Zend/tests/flexible-',
];

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
        $code = stripTrailingWhitespace($code);
        // TODO: Avoid this for now.
        // $code = reindentToTabs($code);
    } else if ($lang === 'php') {
        $code = stripTrailingWhitespace($code);
        $code = reindentToSpaces($code);
    } else if ($lang === 'phpt') {
        $code = transformTestCode($code, function(string $code) {
            $code = stripTrailingWhitespace($code);
            $code = reindentToSpaces($code);
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
        '/(--(?:FILE|SKIPIF|CLEAN)--)(.+?)(?=--[A-Z_]+--)/s',
        function(array $matches) use($transformer) {
            return $matches[1] . $transformer($matches[2]);
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
    case 'inc':
        return 'php';
    case 'phpt':
        return 'phpt';
    default:
        return null;
    }
}
