#!/usr/bin/env php
<?php declare(strict_types=1);

/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://php.net/license/3_01.txt                                     |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Peter Kokot <petk@php.net>                                  |
  +----------------------------------------------------------------------+
*/

function help(): string
{
    return <<<HELP
  *NAME*
    tidy.php - tidy whitespace and end of lines

  *SYNOPSIS*
    php tidy.php [OPTION...] input-path

  *DESCRIPTION*
    Those with a keen eye will notice a recurring whitespace issue in text
    files. For example, redundant trailing whitespace, different end of line
    (EOL) style, missing final EOL, and similar. Some text editors sort this out
    automatically when saving files, some use .editorconfig and some leave files
    intact. This script tidies given files to have consistency and homogeneity
    which brings less unrelated whitespace diffs in Git commits and a better
    coding experience in text editors.

    POSIX defines a line as a sequence of zero or more *non-'<newline>'*
    characters plus a terminating *'<newline>'* character.

    C89 and later standards also mention a final newline: "A source file that is
    not empty shall end in a new-line character, which shall not be immediately
    preceded by a backslash character."

    All non-empty text files have normally at least one final EOL character at
    the end of the file:
    - *LF (\\n)* (\*nix and Mac, default)
    - *CRLF (\\r\\n)* (Windows)
    - *CR (\\r)* (old Mac, obsolete and not used anymore)

    Git stores files in index with LF line endings unless a CRLF is tracked
    specifically. To see EOLs in the repository index and current working tree:
    `git ls-files --eol`

    Git provides several configuration options which users can set on their own
    and define how the EOL is normalized on checkouts in the working tree and
    on commits to the repository:
    - *core.autocrlf = false* or unspecified
      This is Git default behavior. The *core.eol* is the EOL character for
      files in the working tree if there is *text* attribute set in the
      .gitattributes. Default *core.eol* is native. When the *text*
      attribute is unspecified, text files checked out from the repository keep
      the original EOL in the working tree. Text files with LF or CRLF committed
      back to the repository aren't normalized to LF and are tracked as are.
    - *core.autocrlf = input*
      This overwrites *core.eol*. Text files checked out from the repository
      keep original EOL in the working tree. Text files with CRLF are normalized
      to LF when committed to the repository.
    - *core.autocrlf = true*
      Text files checked out from the repository are converted to CRLF in the
      working tree. When committed back to the repository, CRLF is normalized to
      LF.

    The .gitattributes relating to EOL and normalization:
    - *eol=lf|crlf*
    - *text=auto|set|unset|unspecified*

    +------------------+----------------+----------+------+----------+---------+
    | core.autocrlf    | core.eol       | text     | eol  | Worktree | Index   |
    +------------------+----------------+----------+------+----------+---------+
    | false,input      | native         |          |      | LF       | LF      |
    | true             | native,lf,crlf |          |      | CRLF     | LF      |
    | true,false,input | native,lf,crlf |          | lf   | LF       | LF      |
    | true,false,input | native,lf,crlf |          | crlf | CRLF     | LF/CRLF |
    | false            | lf             |          |      | LF       | LF      |
    | false            | crlf           |          |      | CRLF     | CRLF    |
    | false            | native         | set,auto |      | native   | LF      |
    | true             | native         | set,auto |      | CRLF     | LF      |
    +------------------+----------------+----------+------+----------+---------+

    To fix files in the Git repository and commit EOL changes to the index, the
    core.autocrlf=false and core.eol=native need to be set.

    For platform agnostic projects a good way is to commit LF to repository for
    all files that don't require a specific EOL. Files that require a specific
    and same EOL on all systems (either LF or CRLF) the *eol* attribute should
    be specified (in this case CRLF files should be committed to repository with
    CRLF). Rare files with mixed EOL (for example, test data), the *-text*
    attribute might be a good option to avoid Git normalization at checkouts on
    systems with core.autocrlf=true.

    To sum up, this is one solution you can aim for at your repository:
    +----------------------+-------+----------------+
    | Required in worktree | Index | .gitattributes |
    +----------------------+-------+----------------|
    | native               | LF    |                |
    | LF                   | LF    | eol=lf         |
    | CRLF                 | CRLF  | eol=crlf       |
    | mixed                | mixed | -text          |
    +----------------------+-------+----------------+

    This script uses some of these Git settings to determine the default EOL,
    otherwise it can be also manually overridden. LF (\\n) is default.

    Known binary files, such as images, and similar are ignored. When working in
    a Git repository, also files in .gitignore are excluded.

    In the php-src root directory generated files, bundled libraries, test data
    files are additionally ignored and script works only on the predefined
    whitelist.

    By default, original files are also backed up (file.ext~).

    PHP test files (*\*.phpt*) have a specific tidying approach for each
    --SECTION-NAME-- unlike other text files. The following list applies for PHP
    test files in particular.

    - `yes`       - applies if it was specified via command option
    - `no`        - option doesn't apply even if it is set via command option
    - `PHP`       - PHP specific tidying rules
    - `crlf<->lf` - \\n and \\r\\n get normalized, obsolete \\r is not converted

    +--------------------------+-----+-------+-----------+-----+-----+
    | PHP TEST SECTION         | -w  | -n -N | -e        | -l  | -s  |
    +--------------------------+-----+-------+-----------+-----+-----+
    | --TEST--                 | yes |  yes  | crlf<->lf | yes | yes |
    | --DESCRIPTION--          | yes |  yes  | crlf<->lf | yes | yes |
    | --CREDITS--              | yes |  yes  | crlf<->lf | yes | yes |
    | --SKIPIF--               | yes |  yes  | crlf<->lf | yes | yes |
    | --INI--                  | yes |  yes  | crlf<->lf | yes | yes |
    | --ENV--                  | yes |  yes  | crlf<->lf | yes | yes |
    | --EXTENSIONS--           | yes |  yes  | crlf<->lf | yes | yes |
    | --COOKIE--               | yes |  yes  | crlf<->lf | yes | yes |
    | --ARGS--                 | yes |  yes  | crlf<->lf | yes | yes |
    | --REDIRECTTEST--         | no  |  yes  | crlf<->lf | yes | no  |
    | --CAPTURE_STDIO--        | yes |  yes  | crlf<->lf | yes | yes |
    | --STDIN--                | no  |  yes  | crlf<->lf | yes | no  |
    | --CGI--                  | yes |  yes  | crlf<->lf | yes | yes |
    | --PHPDBG--               | yes |  yes  | crlf<->lf | yes | yes |
    | --XFAIL--                | yes |  yes  | crlf<->lf | yes | yes |
    | --CLEAN--                | PHP |  yes  | crlf<->lf | yes | PHP |
    | --POST--                 | no  |  yes  | crlf<->lf | yes | no  |
    | --POST_RAW--             | no  |  yes  | crlf<->lf | yes | no  |
    | --GZIP_POST--            | no  |  yes  | crlf<->lf | yes | no  |
    | --DEFLATE_POST--         | no  |  yes  | crlf<->lf | yes | no  |
    | --PUT--                  | yes |  yes  | crlf<->lf | yes | yes |
    | --GET--                  | yes |  yes  | crlf<->lf | yes | yes |
    | --FILE--                 | PHP |  yes  | crlf<->lf | yes | PHP |
    | --FILEEOF--              | PHP |  yes  | crlf<->lf | yes | PHP |
    | --FILE_EXTERNAL--        | yes |  yes  | crlf<->lf | yes | yes |
    | --EXPECT--               | no  |  yes  | crlf<->lf | yes | no  |
    | --EXPECTF--              | no  |  yes  | crlf<->lf | yes | no  |
    | --EXPECTREGEX--          | no  |  yes  | crlf<->lf | yes | no  |
    | --EXPECT_EXTERNAL--      | yes |  yes  | crlf<->lf | yes | yes |
    | --EXPECTF_EXTERNAL--     | yes |  yes  | crlf<->lf | yes | yes |
    | --EXPECTREGEX_EXTERNAL-- | yes |  yes  | crlf<->lf | yes | yes |
    | --EXPECTHEADERS--        | no  |  yes  | crlf<->lf | yes | no  |
    +--------------------------+-----+-------+-----------+-----+-----+

  *OPTIONS*
    `-h, --help`
          Display this help.

    `-w, --trim-trailing-whitespace`
          Trim all trailing whitespace (spaces and tabs).
    `-N, --trim-final-newlines=[max]`
          Trim redundant final newlines. The *max* value is maximum allowed
          trailing final newlines. Default *max=1*
    `-n, --insert-final-newline`
          Insert a final newline at the end of the file if missing. Default EOL
          character is LF. If file has multiple different EOL characters (LF,
          CRLF, or CR), the prevailing EOL is used.
    `-e, --eol=[newline]`
          Convert the EOL sequence. The default EOL character is determined by
          Git, otherwise falls back to LF. It can be manually set with the
          *newline* value of LF or CRLF.
    `-l, --trim-leading-newlines`
          Trim all redundant newlines at the beginning of the file.
    `-s, --clean-space-before-tab`
          Clean all spaces before tabs in the initial indent part of the lines.

    `-o, --no-backup`
          Disable backed up files.
    `-f, --fix`
          Edit and save files in place.
    `-q, --quiet`
          No output messages. Enables also non-interactive mode.
    `-y, --yes`
          Non-interactive mode, auto yes used for all prompts.
    `-v|vv|vvv, --verbose`
          With more verbosity, additional information are reported about files
          such as non-ascii and non-utf8 file encodings, CR EOLs in phpt files,
          and debugging info such as files that passed the tidying rules and
          ignored files.
    `    --no-colors`
          Disable colors in the output.

  *EXAMPLES*
    `php tidy.php -f path`              Tidy path with all rules enabled
    `php scripts/dev/tidy.php .`        Check the php-src repo
    `php scripts/dev/tidy.php -fo .`    Tidy php-src, no backups
    `php tidy.php -wf path`             Trim trailing whitespace
    `php tidy.php -wnfN path`           Tidy trailing whitespace and final EOLs
    `php tidy.php -N=2 -f path`         Trim final EOL, allow up to 2 final EOL
    `php tidy.php -e=LF -f path`        Convert newlines to LF style
    `php tidy.php -vv path`             Check and list more info about files

  Latest version <https://git.php.net/?p=php-src.git;a=blob;f=scripts/dev/tidy.php>
  Report bugs to <https://bugs.php.net>
HELP;
}

/**
 * Exclude these file patterns from processing.
 */
function getBlacklist(): array
{
    $opt = options();

    $blacklist = [
        // Version control files
        '/(^|\/)\.git\/.+$/',
        '/(^|\/)\.svn\/.+$/',

        // Known binary files based on extension
        '/^.+\.(7z|a|ai|bmp|bz2|data|diff|dll|eot|eps|exe)$/',
        '/^.+\.(gd|gd2|gif|gz|ico|iff)$/',
        '/^.+\.(jar|jp2|jpc|jpeg|jpg)$/',
        '/^.+\.(la|lo|mo|mov|mp3|mp4|msi|o|obj|odt|ole|otf)$/',
        '/^.+\.(patch|pdf|phar|phar\.php|png|ppt|psd|rtf)$/',
        '/^.+\.(so|svg|swf|tar|tar\.gz|tgz|tif|tiff|ttf)$/',
        '/^.+\.(wbmp|webp|woff|woff2)$/',
        '/^.+\.(zip)$/',

        // Backup and hidden files
        '/^.+\~$/',
    ];

    // When working tree is not php-src repository
    if (!isThisPhpSrc($opt['path'])) {
        return $blacklist;
    }

    // The php-src repository specific blacklist
    return array_merge($blacklist, [
        // Generated from other files
        '/^Zend\/zend\_language\_scanner\.c$/',
        '/^Zend\/zend\_ini\_scanner\.c$/',
        '/^Zend\/zend\_vm\_execute\.h$/',
        '/^Zend\/zend\_vm\_opcodes\.h$/',
        '/^Zend\/zend\_vm\_opcodes\.c$/',
        '/^sapi\/cli\/mime\_type\_map\.h$/',
        '/^sapi\/phpdbg\/phpdbg\_lexer\.c$/',
        '/^ext\/fileinfo\/data\_file\.c$/',
        '/^ext\/json\/json\_scanner\.c$/',
        '/^ext\/mbstring\/unicode\_data\.h$/',
        '/^ext\/pdo\/pdo_sql_parser\.c$/',
        '/^ext\/pdo_mysql\/php\_pdo\_mysql\_sqlstate.h$/',
        '/^ext\/phar\/phar\_path\_check\.c$/',
        '/^ext\/standard\/credits\_ext\.h$/',
        '/^ext\/standard\/credits\_sapi\.h$/',
        '/^ext\/standard\/html\_tables\.h$/',
        '/^ext\/standard\/url\_scanner\_ex\.c$/',
        '/^ext\/standard\/var\_unserializer\.c$/',
        '/^win32\/cp_enc_map\.c$/',

        // Bundled libraries maintained in the upstream repositories
        '/^ext\/date\/lib/',
        '/^ext\/fileinfo\/libmagic/',
        '/^ext\/gd\/libgd/',
        '/^ext\/mbstring\/oniguruma/',
        '/^ext\/pcre\/pcre2lib/',

        // Data
        '/^ext\/standard\/html\_tables\/mappings/',

        // Files maintained via Autotools
        '/^TSRM\/m4\/ax\_func\_which\_gethostbyname\_r\.m4$/',
        '/^build\/libtool\.m4$/',
        '/^build\/ax\_check\_compile\_flag\.m4$/',
        '/^build\/ax\_gcc\_func\_attribute\.m4$/',
        '/^build\/shtool$/',
        '/^ltmain\.sh$/',
        '/^config\.guess$/',
        '/^config\.sub$/',

        // Visual Studio generated templates
        '/^win32\/php7dllts\.rc$/',
        '/^win32\/php7ts\.rc$/',
        '/^win32\/php7ts\_cli\.rc$/',

        // Known file types to exclude
        '/^.*tests\/.*\.txt$/',
        '/^.*tests\/.*\.res$/',
        '/^.*tests\/.*\.xsl$/',
        '/^.*tests\/.*\.wsdl$/',

        // Windows builds specific ignores
        '/^Release.*/',
        '/^Debug.*/',
        '/^x64.*/',

        // PHP 7.1 specific
        '/^stub\.c$/',
    ]);
}

/**
 * Filter and include these files in processing after applying blacklist.
 */
function getWhitelist(): array
{
    $opt = options();

    // When working tree is not php-src repository, whitelist all files by default
    if (!isThisPhpSrc($opt['path'])) {
        return ['/^.+$/'];
    }

    // The php-src repository specific whitelist
    return [
        // Known file types based on extension
        '/^.+\.(ac|awk|bat|c|cpp|def|frag|gcov|global|h|html|in|inc|ini)$/',
        '/^.+\.(js|l|m4|macros|md|mk|php|phpt|pl|po|re|rss|sh|txt)$/',
        '/^.+\.(w32|xml|y|yml)$/',

        // All files in the root directory
        '/^[^\/\\\\]*$/',

        // Directories
        '/^build\/.+$/',
        '/^ext\/bcmath\/libbcmath\/.+$/',
        '/^ext\/mbstring\/libmbfl\/.+$/',
        '/^scripts\/.+$/',
        '/^win32\/.+$/',

        // Common files
        '/CREDITS$/',
        '/EXPERIMENTAL$/',
        '/NEWS$/',
        '/LICENSE$/',
        '/README.*$/',
        '/\.gdbinit$/',
        '/\.phpdbginit$/',

        // Known files
        '/^Zend\/buildconf$/',
        '/^Zend\/header$/',
        '/^Zend\/Makefile\.am$/',
        '/^Zend\/zend\_dtrace\.d$/',
        '/^Zend\/zend\_vm\_execute\.skl$/',
        '/^ext\/intl\/ERROR\.CONVENTIONS$/',
        '/^ext\/intl\/tests\/resourcebundle\.build$/',
        '/^ext\/oci8\/oci8\_dtrace\.d$/',
        '/^ext\/phar\/gdbhelp$/',
        '/^ext\/snmp\/tests\/bigtest$/',
        '/^sapi\/phpdbg\/phpdbg\.init\.d$/',
    ];
}

/**
 * Get options from the command line arguments.
 */
function options(array $argv = []): array
{
    static $opt;

    if (isset($opt)) {
        return $opt;
    }

    $shortOptions = 'hwnN::e::lsfoqyv';

    $longOptions = [
        'help',
        'trim-trailing-whitespace',
        'trim-final-newlines::',
        'insert-final-newline',
        'eol::',
        'trim-leading-newlines',
        'clean-space-before-tab',
        'no-colors',
        'fix',
        'no-backup',
        'quiet',
        'yes',
        'verbose',
    ];

    $optionsIndex = null;
    $cliOptions = getopt($shortOptions, $longOptions, $optionsIndex);

    $opt = [
        'fix' => false,
        'trim_trailing_whitespace' => false,
        'trim_final_newlines' => false,
        'max_newlines' => 1,
        'insert_final_newline' => false,
        'eol' => false,
        'default_eol' => null,
        'trim_leading_newlines' => false,
        'clean_space_before_tab' => false,
        'colors' => true,
        'backup' => true,
        'quiet' => false,
        'path' => null,
        'help' => false,
        'invalid' => false,
        'yes' => false,
        'verbose' => 0,
    ];

    foreach ($cliOptions as $option => $value) {
        switch ($option) {
            case 'h':
            case 'help':
                $opt['help'] = true;
            break;
            case 'w':
            case 'trim-trailing-whitespace':
                $opt['trim_trailing_whitespace'] = true;
            break;
            case 'N':
            case 'trim-final-newlines':
                $opt['trim_final_newlines'] = true;
                if (false !== $value && preg_match('/^[0-9]+$/', $value)) {
                    $opt['max_newlines'] = (int) $value;
                }
            break;
            case 'n':
            case 'insert-final-newline':
                $opt['insert_final_newline'] = true;
            break;
            case 'e':
            case 'eol':
                if (is_string($value) && in_array(strtolower($value), ['lf', 'crlf'], true)) {
                    $default = ['lf' => "\n", 'crlf' => "\r\n"];
                    $opt['default_eol'] = $default[strtolower($value)];
                }
                $opt['eol'] = true;
            break;
            case 'l':
            case 'trim-leading-newlines':
                $opt['trim_leading_newlines'] = true;
            break;
            case 's':
            case 'clean-space-before-tab':
                $opt['clean_space_before_tab'] = true;
            break;
            case 'f':
            case 'fix':
                $opt['fix'] = true;
            break;
            case 'no-colors':
                $opt['colors'] = false;
            break;
            case 'o':
            case 'no-backup':
                $opt['backup'] = false;
            break;
            case 'q':
            case 'quiet':
                $opt['quiet'] = true;
                $opt['yes'] = true;
            break;
            case 'y':
            case 'yes':
                $opt['yes'] = true;
            break;
            case 'v':
                $opt['verbose'] = (is_array($value)) ? count($value) : 1;
            break;
            case 'verbose':
                $opt['verbose'] = 3;
            break;
        }
    }

    if (count($argv) < 2) {
        $opt['help'] = true;
    }

    $opt['invalid'] = getInvalidOptions($shortOptions, $longOptions, $argv);

    foreach (array_slice($argv, $optionsIndex) as $file) {
        if (file_exists($file) && $file !== $argv[0]) {
            $opt['path'] = realpath($file);

            break;
        }
    }

    // By default enable all fixing rules if none were specified
    if (
        !$opt['trim_trailing_whitespace']
        && !$opt['insert_final_newline']
        && !$opt['trim_final_newlines']
        && !$opt['eol']
        && !$opt['trim_leading_newlines']
        && !$opt['clean_space_before_tab']
    ) {
        $opt['trim_trailing_whitespace'] = true;
        $opt['insert_final_newline'] = true;
        $opt['trim_final_newlines'] = true;
        $opt['eol'] = true;
        $opt['trim_leading_newlines'] = true;
        $opt['clean_space_before_tab'] = true;
    }

    // Disable colors when redirecting output to file ./tidy.php > file
    if (\function_exists('stream_isatty')) {
        $opt['colors'] = (stream_isatty(STDOUT)) ? $opt['colors'] : false;
    }

    // Check options and system dependencies
    check($opt);

    return $opt;
}

/**
 * Get all invalid command line options.
 */
function getInvalidOptions(string $shortOptions, array $longOptions, array $argv): ?string
{
    $unknown = [];

    // Build regex for validating short options
    $regex = '/^\-[';

    // Options without values
    if (preg_match_all('/[a-z0-9](?!\:{1,2})/i', $shortOptions, $matches)) {
        foreach ($matches[0] as $match) {
            $regex .= $match[0];
        }
    }

    $regex .= ']*(';

    // Options with values
    if (preg_match_all('/([a-z0-9])\:{1,2}/i', $shortOptions, $matches)) {
        foreach ($matches[1] as $match) {
            $regex .= $match[0].'.*$|';
        }
    }

    $regex .= '$)/';

    foreach ($argv as $i => $argument) {
        // Discover possible unknown long passed options
        if (preg_match('/^\-\-([a-z\-]+)\=?.*/i', $argument, $matches)
            && !in_array($matches[1], str_replace(':', '', $longOptions), true)
        ) {
            $unknown[] = $argument;
        }

        // Discover possible unknown short passed options
        if (preg_match('/^\-([a-z0-9]+).*$/i', $argument) && !preg_match($regex, $argument)) {
            $unknown[] = $argument;
        }
    }

    if (!empty($unknown)) {
        return "invalid option(s) '".implode($unknown, ', ')."'";
    }

    return null;
}

/**
 * Exit on failed checks or outputs warnings about missing system dependencies.
 */
function check(array $opt): void
{
    // Check for help
    if ($opt['help']) {
        output(help(), false);

        exit;
    }

    // Invalid option
    if ($opt['invalid']) {
        output(invalid($opt['invalid']), false);

        exit(1);
    }

    // Check if path is set
    if (empty($opt['path'])) {
        output(invalid('missing path'), false);

        exit(1);
    }

    // Prompt user for input
    if ($opt['fix'] && !$opt['yes']) {
        prompt();
    }

    // The Tokenizer extension provides better PHP specific code tidying
    if (!extension_loaded('tokenizer')) {
        output('**WARN**: Enable tokenizer extension for better PHP code tidying.');
    }

    // The mbstring extension is used for reporting the file encodings
    if (!extension_loaded('mbstring')) {
        output('**WARN**: Enable mbstring extension for reporting file encodings.');
    }
}

/**
 * Guess if provided path is the root of the php-src repository.
 */
function isThisPhpSrc(string $path): bool
{
    $checks = ['build', 'ext', 'main/php.h', 'Zend'];

    foreach (preg_filter('/^/', $path.'/', $checks) as $check) {
        if (!file_exists($check)) {
            return false;
        }
    }

    return true;
}

/**
 * Output message based on given options. For colors, Markdown based code can be
 * replaced. Double asterisk ** starts and ends red color, grave accent `
 * starts and ends green color, single asterisk not preceded by backslash *
 * starts and ends yellow color.
 */
function output(?string $message = null, ?bool $quiet = null): void
{
    $opt = options();

    if ($quiet ?? $opt['quiet']) {
        return;
    }

    $red = ($opt['colors']) ? "\e[31m" : '';
    $green = ($opt['colors']) ? "\e[32m" : '';
    $yellow = ($opt['colors']) ? "\e[33m" : '';
    $default = ($opt['colors']) ? "\e[0m" : '';

    // Replace ` characters with green color code
    $cur = $default;
    foreach (str_split($message) as $char) {
        $cur = ($cur === $default) ? $green : $default;
        $message = preg_replace('/\`/m', $cur, $message, 1);
    }

    // Replace ** characters with red color code
    $cur = $default;
    foreach (str_split($message) as $char) {
        $cur = ($cur === $default) ? $red : $default;
        $message = preg_replace('/\*\*/m', $cur, $message, 1);
    }

    // Replace single asterisk * characters with yellow color code
    $cur = $default;
    foreach (str_split($message) as $char) {
        $cur = ($cur === $default) ? $yellow : $default;
        $message = preg_replace('/(?<!\\\\)\*/m', $cur, $message, 1);
    }

    // Replace escaped characters
    $message = preg_replace('/\\\\\*/m', '*', $message);

    echo $message."\n";
}

/**
 * Short invalid usage error.
 */
function invalid(string $error = 'INVALID USAGE'): string
{
    return 'tidy.php: **'.$error."**\nTry '*php tidy.php --help*' for more info.";
}

/**
 * Prompt user to continue executing script or exit.
 */
function prompt(): void
{
    $opt = options();

    output('This will overwrite'.(is_dir($opt['path']) ? ' all files in' : '').' *'.$opt['path'].'*');
    output('Backup copies '.($opt['backup'] ? 'will' : '**will NOT**')." be created.\n");
    output('Do you want to continue `[yes|no]`?');

    $handle = fopen('php://stdin', 'r');
    $line = fgets($handle);

    if (!in_array(trim((strtolower($line))), ['yes', 'y', '1'], true)) {
        output('**ABORTING**');

        exit;
    }
}

/**
 * Trim trailing spaces and tabs from each line including or without the final
 * newline trimmed.
 */
function trimTrailingWhitespace(string $content): string
{
    return preg_replace('/(*BSR_ANYCRLF)[\t ]+(\R|$)/m', '$1', $content);
}

/**
 * Trim all newlines from the beginning of the file.
 */
function trimLeadingNewlines(string $content): string
{
    return ltrim($content, "\r\n");
}

/**
 * Insert one missing final newline at the end of the string, using a prevailing
 * EOL from the given string - LF (\n), CRLF (\r\n) or CR (\r).
 */
function insertFinalNewline(string $content): string
{
    if (!in_array(substr($content, -1), ["\n", "\r"], true)) {
        $content .= getPrevailingEol($content);
    }

    return $content;
}

/**
 * Get all final newlines.
 */
function getFinalNewlines(string $content): array
{
    $newlines = [];

    while ('' !== $content) {
        if ("\r\n" === substr($content, -2)) {
            $newlines[] = "\r\n";
            $content = substr($content, 0, -2);
            continue;
        }

        if ("\n" === substr($content, -1)) {
            $newlines[] = "\n";
            $content = substr($content, 0, -1);
            continue;
        }

        if ("\r" === substr($content, -1)) {
            $newlines[] = "\r";
            $content = substr($content, 0, -1);
            continue;
        }

        break;
    }

    return array_reverse($newlines);
}

/**
 * Trim multiple final newlines and allow up to $max trailing newlines.
 */
function trimFinalNewlines(string $content, int $max = 1): string
{
    $newlines = getFinalNewlines($content);
    $trimmed = rtrim($content, "\r\n");

    for ($i = 0; $i < $max; ++$i) {
        if (empty($newlines[$i])) {
            break;
        }

        $trimmed .= $newlines[$i];
    }

    return $trimmed;
}

/**
 * Get end of line based on the prevailing LF, CRLF or CR newline characters.
 */
function getPrevailingEol(string $content): string
{
    // Empty files don't need a newline attached
    if ('' === $content) {
        return '';
    }

    // Match all LF, CRLF and CR EOL characters
    preg_match_all('/(*BSR_ANYCRLF)\R/', $content, $matches);

    // For single line files the default EOL is returned
    if (empty($matches[0])) {
        return getDefaultEol();
    }

    $counts = array_count_values($matches[0]);
    arsort($counts);
    $prevailingEol = key($counts);

    return $prevailingEol;
}

/**
 * Get EOL sequence for the given content.
 */
function getEols(string $content): string
{
    $eols = ['lf' => 0, 'crlf' => 0, 'cr' => 0];

    // Match all LF, CRLF and CR EOL characters
    preg_match_all('/(*BSR_ANYCRLF)\R/', $content, $matches);

    foreach ($matches[0] as $match) {
        if ("\n" === $match) {
            ++$eols['lf'];
        } elseif ("\r\n" === $match) {
            ++$eols['crlf'];
        } elseif ("\r" === $match) {
            ++$eols['cr'];
        }
    }

    $message = ($eols['lf'] > 0) ? $eols['lf'].' LF ' : '';
    $message .= ($eols['crlf'] > 0) ? $eols['crlf'].' CRLF ' : '';
    $message .= ($eols['cr'] > 0) ? $eols['cr'].' CR ' : '';

    return $message;
}

/**
 * Convert all EOL characters to default EOL.
 */
function convertEol(string $content, $file, bool $enableCr = false): string
{
    if ($enableCr) {
        return preg_replace('/(*BSR_ANYCRLF)\R/m', getDefaultEol($file), $content);
    }

    return preg_replace('/(?>\r\n|\n)/m', getDefaultEol($file), $content);
}

/**
 * Get the default EOL character by checking a value of command line option --eol,
 * otherwise use Git configuration. When Git is not used, the default EOL is LF.
 */
function getDefaultEol(?string $file = null): string
{
    $opt = options();

    if (!empty($opt['default_eol'])) {
        return $opt['default_eol'];
    }

    // Default EOL is *nix style LF (\n) character
    $eol = "\n";

    if (null === $file || !hasGit()) {
        return $eol;
    }

    if (in_array(relative($file), getCrlfFiles($opt['path'], true))) {
        return "\r\n";
    }

    return $eol;
}

/**
 * Files with eol=crlf Git attribute should have CRLF line endings, others LF.
 */
function getCrlfFiles(string $path): array
{
    static $files;

    if (isset($files)) {
        return $files;
    }

    $files = shell_exec(sprintf('cd %s && git ls-files -z --eol 2>&1', escapeshellarg($path)));
    $files = explode("\0", trim($files ?? ''));
    $files = array_filter($files, function ($item) {
        return preg_match('/^i\/crlf.*[ ]+w\/.*attr\/.*eol=crlf.*$/', $item);
    });
    $files = preg_filter('/^i\/.*w\/.*attr\/.*[ \t]+/', '', $files);

    return $files;
}

/**
 * Clean all spaces in front of the tabs in the initial indent part of the lines.
 */
function cleanSpaceBeforeTab(string $content): string
{
    $regex = '/^(\t*)([ ]+)(\t+)/m';

    while (preg_match($regex, $content, $matches)) {
        $content = preg_replace($regex, '$1$3', $content);
    }

    return $content;
}

/**
 * Helper for checking if given path is a Git repository or is in the Git
 * repository, and if Git is present on current system.
 */
function hasGit(?string $path = null): bool
{
    static $hasGit;

    if (isset($hasGit)) {
        return $hasGit;
    }

    exec('git --version 2>&1', $output, $exitCode);
    if (0 !== $exitCode) {
        return $hasGit = false;
    }

    if (null !== $path && is_dir($path) && file_exists($path.'/.git')) {
        return $hasGit = true;
    }

    return $hasGit = false;
}

/**
 * Get filtered files from a given path. Files in .gitignore are omitted.
 */
function getFiles(): array
{
    $opt = options();

    if (is_file($opt['path'])) {
        return [$opt['path']];
    }

    if (hasGit($opt['path'])) {
        $files = shell_exec(sprintf('cd %s && git ls-tree -r -z HEAD --name-only', escapeshellarg($opt['path'])));
        $files = explode("\0", trim($files));
        $files = preg_filter('/^/', $opt['path'].'/', $files);
    } else {
        $innerIterator = new \RecursiveDirectoryIterator($opt['path'], \RecursiveDirectoryIterator::SKIP_DOTS);
        $iterator = new \RecursiveIteratorIterator($innerIterator);

        $files = [];

        foreach ($iterator as $file) {
            if ($file->isDir()) {
                continue;
            }

            $files[] = $file->getPathname();
        }
    }

    // Remove blacklist matches
    foreach (getBlacklist() as $regex) {
        $data = array_filter($files, function ($item) use ($regex, $opt) {
            $blacklisted = preg_match($regex, relative($item));

            if ($blacklisted && $opt['verbose'] >= 3) {
                output('*SKIP* '.relative($item).': *ignored*');
            }

            return $blacklisted;
        });

        $files = array_diff($files, $data);
    }

    // Filter files to match the whitelist
    $filtered = [];
    foreach (getWhitelist() as $regex) {
        $data = array_filter($files, function ($item) use ($regex) {
            return preg_match($regex, relative($item));
        });

        $filtered = array_merge($filtered, array_diff($data, $filtered));
    }

    return $filtered;
}

/**
 * Get file name relative to the given input path root project directory.
 */
function relative(string $file): string
{
    $opt = options();

    return str_replace($opt['path'].'/', '', $file);
}

/**
 * Tidy content for a file with optional given overridden rules.
 */
function tidyContent(string $content, string $file, array $rules = []): array
{
    $opt = array_merge(options(), $rules);

    $buffer = $content;
    $logs = [];

    if ($opt['trim_trailing_whitespace']) {
        $buffer = trimTrailingWhitespace($buffer);
    }

    if ($buffer !== $content) {
        $logs[] = 'trailing whitespace';
        $content = $buffer;
    }

    if ($opt['trim_final_newlines']) {
        $buffer = trimFinalNewlines($buffer, $opt['max_newlines']);
    }

    if ($buffer !== $content) {
        $eols = getFinalNewlines($content);
        $logs[] = count($eols).' final EOL(s)';
        $content = $buffer;
    }

    if ($opt['insert_final_newline']) {
        $buffer = insertFinalNewline($buffer);
    }

    if ($buffer !== $content) {
        $logs[] = 'missing final EOL';
        $content = $buffer;
    }

    if ($opt['eol']) {
        $buffer = convertEol($buffer, $file, true);
    }

    if ($buffer !== $content) {
        $logs[] = getEols($content).'line terminators';
        $content = $buffer;
    }

    if ($opt['trim_leading_newlines']) {
        $buffer = trimLeadingNewlines($buffer);
    }

    if ($buffer !== $content) {
        $logs[] = 'leading newlines';
        $content = $buffer;
    }

    if ($opt['clean_space_before_tab']) {
        $buffer = cleanSpaceBeforeTab($buffer);
    }

    if ($buffer !== $content) {
        $logs[] = 'space before tab';
        $content = $buffer;
    }

    return [$content, $logs];
}

/**
 * Overwrite file and save a backup copy.
 */
function save(string $file, string $content): bool
{
    $opt = options();

    // Backup file
    if ($opt['backup'] && false !== file_put_contents($file.'~', $content)) {
        output('`COPY`  '.relative($file).'~: *backup copy saved*');
    }

    if (false === file_put_contents($file, $content)) {
        output('**FAIL**  '.relative($file).': **file could not be saved**');

        return false;
    }

    return true;
}

/**
 * Explode PHP test file sections to array.
 */
function getTestSections(string $file): array
{
    $fp = fopen($file, 'rb') or exit("Cannot open test file: $file");

    $sections = [];
    $section = '';

    while (!feof($fp)) {
        $line = fgets($fp);

        if (false === $line) {
            break;
        }

        if (preg_match('/^--[_A-Z]+--.*/', $line, $matches)) {
            $section = $line;
            $sections[$section] = '';
        } elseif ('' !== $section) {
            $sections[$section] .= $line;
        }
    }

    return $sections;
}

/**
 * Merge given PHP test sections back to file contents.
 */
function mergeTestSections(array $sections): string
{
    $content = '';

    foreach ($sections as $name => $section) {
        $content .= $name.$section;
    }

    return $content;
}

/**
 * PHP test files need special tidying approach for each section.
 */
function tidyPhpTestFile(string $file): array
{
    $opt = options();
    $logs = [];
    $sections = [];

    // Sections specific fixes
    foreach (getTestSections($file) as $name => $section) {
        $nameBuffer = $name;
        $buffer = $section;

        // Section name contains entire line with EOL character so we trim it
        $sectionRealName = trim($nameBuffer);

        // Trim trailing whitespace after section names
        if ($opt['trim_trailing_whitespace']) {
            $nameBuffer = trimTrailingWhitespace($nameBuffer);
        }

        if ($nameBuffer !== $name) {
            $logs[] = $sectionRealName.' trailing whitespace';
        }

        // Sections specific tidying
        if (in_array($sectionRealName, [
            '--FILE--',
            '--FILEEOF--',
            '--CLEAN--',
            ], true)) {
            if ($opt['trim_trailing_whitespace']) {
                $buffer = trimTrailingWhitespaceFromPhp($buffer);
            }

            if ($buffer !== $section) {
                $logs[] = $sectionRealName.' trailing whitespace';
                $section = $buffer;
            }

            if ($opt['clean_space_before_tab']) {
                $buffer = cleanSpaceBeforeTabFromPhp($buffer);
            }

            if ($buffer !== $section) {
                $logs[] = $sectionRealName.' space before tab';
                $section = $buffer;
            }
        } elseif (in_array($sectionRealName, [
            '--TEST--',
            '--DESCRIPTION--',
            '--CREDITS--',
            '--SKIPIF--',
            '--INI--',
            '--ENV--',
            '--EXTENSIONS--',
            '--COOKIE--',
            '--ARGS--',
            '--CAPTURE_STDIO--',
            '--CGI--',
            '--PHPDBG--',
            '--XFAIL--',
            '--PUT--',
            '--GET--',
            '--FILE_EXTERNAL--',
            '--EXPECT_EXTERNAL--',
            '--EXPECTF_EXTERNAL--',
            '--EXPECTREGEX_EXTERNAL--',
            '--EXPECTHEADERS--',
            ], true)
        ) {
            if ($opt['trim_trailing_whitespace']) {
                $buffer = trimTrailingWhitespace($buffer);
            }

            if ($buffer !== $section) {
                $logs[] = $sectionRealName.' trailing whitespace';
                $section = $buffer;
            }

            if ($opt['clean_space_before_tab']) {
                $buffer = cleanSpaceBeforeTab($buffer);
            }

            if ($buffer !== $section) {
                $logs[] = $sectionRealName.' space before tab';
                $section = $buffer;
            }
        }

        // Override tidy rules for all test sections
        $rules = [
            'trim_trailing_whitespace' => false,
            'clean_space_before_tab' => false,
            'eol' => false,
        ];

        // Each test section need to have at least one final newline
        if (0 === $opt['max_newlines']) {
            $rules['max_newlines'] = 1;
        }

        list($buffer, $bufferLogs) = tidyContent($buffer, $file, $rules);

        // Add test section name to each log entry
        $bufferLogs = preg_filter('/^/', $sectionRealName.' ', $bufferLogs);

        $logs = array_merge($logs, $bufferLogs);

        $sections[$nameBuffer] = $buffer;
    }

    // Entire file content specific fixes
    $content = $buffer = mergeTestSections($sections);

    // Convert EOL for the entire phpt file
    if ($opt['eol']) {
        // Disable converting CR since it might be part of a test
        $buffer = convertEol($buffer, $file, false);

        if ($opt['verbose'] >= 1 && $buffer !== convertEol($buffer, $file, true)) {
            output('*WARN*  '.relative($file).': *'.getEols($content).'line terminators*');
        }
    }

    if ($buffer !== $content) {
        $logs[] = getEols($content).'line terminators';
        $content = $buffer;
    }

    return [$content, $logs];
}

/**
 * Output log messages and save file.
 */
function tidyFile(string $file): bool
{
    $opt = options();

    $original = file_get_contents($file);

    if ('.phpt' === substr($file, -5)) {
        if ('--TEST--' !== substr($original, 0, 8)) {
            output('**FAIL**  '.relative($file).': **invalid PHP test**');

            return false;
        }

        list($cleaned, $logs) = tidyPhpTestFile($file);
    } else {
        list($cleaned, $logs) = tidyContent($original, $file);
    }

    // Report file encoding
    if ($opt['verbose'] >= 2) {
        $encoding = getFileEncoding($cleaned, $file);
        if (!in_array($encoding, ['ascii', 'us-ascii', 'utf-8'], true)) {
            output('*WARN*  '.relative($file).': *encoding '.$encoding.'*');
        }
    }

    if ($cleaned !== $original) {
        countFixed();

        if ($opt['fix']) {
            output('`FIXED` '.relative($file).': `'.implode(', ', array_unique($logs)).'`');

            return save($file, $cleaned);
        }

        output('**FAIL**  '.relative($file).': **'.implode(', ', array_unique($logs)).'**');

        return false;
    }

    if ($opt['verbose'] >= 3) {
        output('`PASS`  '.relative($file));
    }

    return true;
}

/**
 * PHP code specific trimming of trailing whitespace.
 */
function trimTrailingWhitespaceFromPhp(string $source): string
{
    // If the tokenizer extension is not enabled several patterns are used.
    if (!extension_loaded('tokenizer')) {
        $patterns = [
            '/(*BSR_ANYCRLF)(\<\?php)[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\\?\>)[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\)[\t ]?\{)[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\)\;)[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\})[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\"\;|\'\;)[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\/\/.*(?<![ \t]))[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(\/\/.*(?<![ \t]))[\t ]+(\R|$)/m',
            '/(*BSR_ANYCRLF)(^)[\t ]+(\R|$)/m',
        ];

        foreach ($patterns as $pattern) {
            $source = preg_replace($pattern, '$1$2', $source);
        }

        return $source;
    }

    return tidyPhpCode($source, function (string $src) {
        return trimTrailingWhitespace($src);
    });
}

/**
 * PHP code specific spaces before tabs cleaning.
 */
function cleanSpaceBeforeTabFromPhp(string $source): string
{
    return tidyPhpCode($source, function (string $src) {
        return cleanSpaceBeforeTab($src);
    });
}

/**
 * Clean PHP code by using a callback and a Tokenizer extension.
 */
function tidyPhpCode(string $source, callable $callback): string
{
    // @ suppresses warning: octal escape sequence overflow \542 is greater than \377
    $tokens = @token_get_all($source);

    $content = '';
    $buffer = '';

    foreach ($tokens as $i => $token) {
        if (is_string($token)) {
            // simple 1-character token
            $buffer .= $token;
        } else {
            list($id, $text) = $token;

            switch ($id) {
                case T_COMMENT:
                    if (!empty($tokens[$i + 1][0]) && T_CLOSE_TAG !== $tokens[$i + 1][0]) {
                        $buffer .= $callback($text);
                    } else {
                        $buffer .= $text;
                    }
                break;
                case T_DOC_COMMENT:
                    $buffer .= $callback($text);
                break;
                case T_START_HEREDOC:
                    $buffer .= $text;
                    $content .= $callback($buffer);

                    $buffer = '';
                break;
                case T_END_HEREDOC:
                    $buffer .= $text;
                    $content .= $buffer;
                    $buffer = '';
                break;
                case T_CONSTANT_ENCAPSED_STRING:
                    $before = (preg_match('/[ \t]+\z/', $buffer, $matches)) ? $matches[0] : '';
                    $content .= $callback($buffer);
                    $after = (preg_match('/[ \t]+\z/', $content, $matches)) ? $matches[0] : '';

                    // When trimming trailing whitespace, the final trailing ws should stay
                    if ($before !== $after && $after === '') {
                        $content .= $before;
                    }

                    $content .= $text;
                    $buffer = '';
                break;
                default:
                    $buffer .= $text;
                break;
            }
        }
    }

    $content .= $callback($buffer);

    return $content;
}

/**
 * Get encoding of the given content or file. If encoding can't be determined
 * from the file content using the mbstring extension, the file path is used
 * and the file command line tool if present.
 */
function getFileEncoding(string $content, string $file): ?string
{
    // Determine file content encoding using mbstring extension
    if (extension_loaded('mbstring')) {
        //check string strict for encoding out of list of supported encodings
        $encoding = mb_detect_encoding($content, mb_list_encodings(), true);

        // UTF-8 or normal ASCII text file
        if (in_array($encoding, ['UTF-8', 'ASCII'], true)) {
            return strtolower($encoding);
        }

        // Some known encoding
        if (false !== $encoding) {
            return strtolower($encoding);
        }
    }

    // Command line file utility
    $output = shell_exec(sprintf('file -i %s 2>&1', escapeshellarg($file)));

    if (isset($output)) {
        $buffer = explode('charset=', $output);

        return isset($buffer[1]) ? $buffer[1] : null;
    }

    return null;
}

/**
 * Internal counter of fixed files for logging output.
 */
function countFixed(): int
{
    static $counter = 0;

    return $counter++;
}

function init(array $argv): int
{
    $time = microtime(true);
    $opt = options($argv);

    output("Executing `tidy.php`\nWorking tree: ".$opt['path']."\n");

    $files = getFiles();
    $exitCode = 0;
    foreach ($files as $file) {
        $exitCode = (tidyFile($file) && 0 === $exitCode) ? 0 : 1;
    }

    $time = round((microtime(true) - $time), 3);

    $output = "\nAll done. `".countFixed().'` file(s) '.($opt['fix'] ? 'fixed' : 'should be fixed');
    $output .= '. Checked *'.count($files).'* file(s) in '.$time.' sec';
    $output .= ' and consumed '.(round(memory_get_peak_usage() / 1024 / 1024, 3)).' MB memory';

    output($output);

    return $exitCode;
}

exit(init($argv));
