#!/usr/bin/env php
<?php

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

function help(): void
{
    $help = <<<HELP
  *NAME*
    tidy.php - tidy whitespace and end of lines

  *SYNOPSIS*
    php tidy.php [OPTION...] input-path

  *DESCRIPTION*
    Those with a keen eye will notice a recurring whitespace issue in text
    files. For example, redundant trailing whitespace, different end of line
    (EOL) style, missing final end of line, and similar. Some text editors sort
    this out automatically when saving files, some use .editorconfig and some
    leave files intact. This script tidies whitespace and end of lines for
    consistency and homogeneity, which brings less unrelated whitespace diffs in
    Git commits and a better development experience in text editors.

    POSIX defines a line as a sequence of zero or more *non-'<newline>'*
    characters plus a terminating *'<newline>'* character.

    C89 and later standards also mention a final newline: "A source file that is
    not empty shall end in a new-line character, which shall not be immediately
    preceded by a backslash character."

    All text files should normally have at least one final EOL character:
    - *LF (\\n)* (\*nix and Mac, default)
    - *CRLF (\\r\\n)* (Windows)
    - *CR (\\r)* (old Mac, obsolete)

    Git stores files in index with LF line endings unless a CRLF is tracked
    specifically. To see EOLs in the repository index and current working tree:
    `git ls-files --eol`

    Git provides several configuration options:
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
      Text files checked out from the repository are converted to CRLF in
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

    To fix files in Git repository and commit EOL changes to index, the
    core.autocrlf=false and core.eol=native need to be set.

    For platform agnostic projects a good way is to commit LF to repository for
    all files that don't require a specific EOL. Files that require a specific
    and same EOL on all systems (either LF or CRLF) the *eol* attribute should
    be specified (CRLF files should be committed to repository in this case with
    CRLF). Files with mixed EOL (for example test data), the *-text* attribute
    might be a good option to avoid Git normalization at checkouts on systems
    with core.autocrlf=true.

    +------------------+-------+----------------+
    | File in worktree | Index | .gitattributes |
    +------------------+-------+----------------|
    | native           | LF    |                |
    | LF               | LF    | eol=lf         |
    | CRLF             | CRLF  | eol=crlf       |
    | mixed            | mixed | -text          |
    +------------------+-------+----------------+

    This script uses some of these Git settings to determine the default EOL,
    otherwise EOL can be manually overridden. By default, the LF (\\n) is used.

    Known binary files such as images are ignored. When working in a Git
    repository, files in .gitignore are excluded.

    In the php-src root directory it additionally ignores generated files,
    bundled libraries, test data files, and works only on the predefined
    whitelist.

    By default, original files are also backed up (file.ext~).

  *OPTIONS*
    `-h, --help`
          Display this help.
    `-w, --trim-trailing-whitespace`
          Trim all trailing whitespace (spaces and tabs).
    `-N, --trim-final-newlines [max]`
          Trim redundant final newlines. The *max* value is maximum allowed
          trailing final newlines. Default *max=1*
    `-n, --insert-final-newline`
          Insert a final newline at the end of the file if missing. Default
          EOL character is LF. If file has multiple different EOL characters
          (LF, CRLF, or CR), the prevailing EOL is used.
    `-e, --eol [newline]`
          Convert the EOL sequence. The default EOL character is determined by
          Git, otherwise falls back to LF. It can be manually set with the
          *newline* value of LF or CRLF.
    `-b, --trim-beginning-whitespace`
          Trim whitespace (spaces and tabs) from beginning of the file.
    `-B, --trim-beginning-newlines`
          Trim all newlines at the beginning of the file.
    `-s, --clean-space-before-tab`
          Clean all spaces before tabs in the initial indent part of the lines.
    `-a, --all`
          Enable all fixing rules.
    `-o, --no-backup`
          Disable backed up files.
    `-f, --fix`
          Edit and save files in place.
    `-q, --quiet`
          No output messages. Enables also non-interactive mode.
    `-y, --yes`
          Non-interactive mode, auto yes used for all prompts.
    `-v, --verbose`
          Increase the number of output information.
    `    --no-colors`
          Disable colors in the output.
    `-p, --php-tests`
          Enable specific *\*.phpt* PHP test files with different tidying rules
          for each --SECTION-NAME-- unlike other text files.
    `-P, --force-php-tests`
          Same as -p and treat *\*.phpt* files as regular text files with
          special cleaning rules for them.

  *EXAMPLES*
    './tidy.php `-f -a path`'          Tidy path with all rules enabled
    './scripts/dev/tidy.php `.`'       Check basic rules in the php-src repo
    './scripts/dev/tidy.php `-f -o .`' Tidy basic rules in php-src, no backups
    './tidy.php `-w -f path`'          Trim trailing whitespace
    './tidy.php `-w -n -N -f path`'    Tidy trailing whitespace and final EOLs
    './tidy.php `-N 2 -f path`'        Trim final EOL, allow up to 2 final EOL
    './tidy.php `-e LF -f path`'       Convert newlines to LF style
    './tidy.php `-v path`'             Check and list all files
HELP;

    output($help, false);

    exit;
}

/**
 * Exclude these patterns from processing.
 */
function getBlacklist(): array
{
    return [
        // Version control files
        '/(^|\/)\.git\/.+$/',
        '/(^|\/)\.svn\/.+$/',

        // Known binary files based on extension
        '/^.+\.(7z|a|ai|bmp|bz2|data|dll|eot|exe|gd|gd2|gif|gz|ico|iff)$/',
        '/^.+\.(jar|jp2|jpc|jpeg|jpg)$/',
        '/^.+\.(la|lo|mo|mov|mp3|mp4|msi|o|obj|odt|ole|otf)$/',
        '/^.+\.(patch|pdf|phar|phar\.php|png|ppt|psd|rtf)$/',
        '/^.+\.(so|svg|swf|tar|tar\.gz|tgz|tif|tiff|ttf)$/',
        '/^.+\.(wbmp|webp|woff|woff2)$/',
        '/^.+\.(zip)$/',

        // Backup and hidden files
        '/^.+\~$/',
    ];
}

/**
 * Filter and include these in processing after applying blacklist.
 */
function getWhitelist(): array
{
    return ['/^.+$/'];
}

/**
 * When in PHP source code repository, also exclude these patterns.
 */
function getPhpBlacklist(): array
{
    return [
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
        '/^ext\/sqlite3\/libsqlite/',

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
    ];
}

/**
 * When in PHP source code, filter and include these after applying blacklist.
 */
function getPhpWhitelist(): array
{
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
 * Get options from the command line arguments. When called next time without
 * arguments, the previously set options are returned with the help of static
 * variable.
 */
function options(array $argv = []): array
{
    // Default configuration options
    static $opt = [
        'fix'                       => false,
        'trim_trailing_whitespace'  => false,
        'trim_final_newlines'       => false,
        'max_newlines'              => 1,
        'insert_final_newline'      => false,
        'eol'                       => false,
        'default_eol'               => null,
        'trim_beginning_whitespace' => false,
        'trim_beginning_newlines'   => false,
        'clean_space_before_tab'    => false,
        'colors'                    => true,
        'backup'                    => true,
        'quiet'                     => false,
        'path'                      => null,
        'help'                      => false,
        'invalid'                   => false,
        'has_git'                   => false,
        'yes'                       => false,
        'verbose'                   => false,
        'php_tests'                 => false,
        'force_php_tests'           => false,
    ];

    if (empty($argv)) {
        return $opt;
    }

    foreach ($argv as $i=>$argument) {
        switch ($argument) {
            case '-h':
            case '--help':
                $opt['help'] = true;
            break;
            case '-w':
            case '--trim-trailing-whitespace':
                $opt['trim_trailing_whitespace'] = true;
            break;
            case '-N':
            case '--trim-final-newlines':
                if (isset($argv[$i+1]) && preg_match('/^[0-9]+/', $argv[$i+1])) {
                    $opt['max_newlines'] = (int) $argv[$i+1];
                    array_splice($argv, $i, 1);
                }
                $opt['trim_final_newlines'] = true;
            break;
            case '-n':
            case '--insert-final-newline':
                $opt['insert_final_newline'] = true;
            break;
            case '-e':
            case '--eol':
                if (isset($argv[$i+1])
                    && in_array(strtolower($argv[$i+1]), ['lf', 'crlf'])
                ) {
                    $default = ['lf' => "\n", 'crlf' => "\r\n"];
                    $opt['default_eol'] = $default[strtolower($argv[$i+1])];
                    array_splice($argv, $i, 1);
                }
                $opt['eol'] = true;
            break;
            case '-b':
            case '--trim-beginning-whitespace':
                $opt['trim_beginning_whitespace'] = true;
            break;
            case '-B':
            case '--trim-beginning-newlines':
                $opt['trim_beginning_newlines'] = true;
            break;
            case '-s':
            case '--clean-space-before-tab':
                $opt['clean_space_before_tab'] = true;
            break;
            case '-f':
            case '--fix':
                $opt['fix'] = true;
            break;
            case '--no-colors':
                $opt['colors'] = false;
            break;
            case '-o':
            case '--no-backup':
                $opt['backup'] = false;
            break;
            case '-q':
            case '--quiet':
                $opt['quiet'] = true;
                $opt['yes'] = true;
            break;
            case '-y':
            case '--yes':
                $opt['yes'] = true;
            break;
            case '-v':
            case '--verbose':
                $opt['verbose'] = true;
            break;
            case '-p':
            case '--php-tests':
                $opt['php_tests'] = true;
            break;
            case '-P':
            case '--force-php-tests':
                $opt['force_php_tests'] = true;
            break;
            case '-a':
            case '-all':
                $opt['trim_trailing_whitespace'] = true;
                $opt['insert_final_newline'] = true;
                $opt['trim_final_newlines'] = true;
                $opt['eol'] = true;
                $opt['trim_beginning_whitespace'] = true;
                $opt['trim_beginning_newlines'] = true;
                $opt['clean_space_before_tab'] = true;
            break;
            default:
                if (count($argv) < 2) {
                    $opt['help'] = true;
                }

                if ((preg_match('/^-.+/', $argument))) {
                    $opt['invalid'] = "invalid option '$argument'";
                }

                if (file_exists($argument) && $argument !== $argv[0]) {
                    $opt['path'] = $argument;
                }
            break;
        }
    }

    // By default enable basic fixing rules if none were specified
    if (
        !$opt['trim_trailing_whitespace']
        && !$opt['insert_final_newline']
        && !$opt['trim_final_newlines']
        && !$opt['eol']
        && !$opt['trim_beginning_whitespace']
        && !$opt['trim_beginning_newlines']
        && !$opt['clean_space_before_tab']
    ) {
        $opt['trim_trailing_whitespace'] = true;
        $opt['insert_final_newline'] = true;
        $opt['trim_final_newlines'] = true;
        $opt['eol'] = true;
    }

    // Simplify edge case '--trim-trailing-newlines 0 --insert-final-newline'
    if ($opt['insert_final_newline'] && $opt['max_newlines'] === 0) {
        $opt['max_newlines'] = 1;
    }

    // Check if Git is available
    exec('git --version 2>&1', $output, $exitCode);
    if (0 === $exitCode && file_exists($opt['path'].'/.git')) {
        $opt['has_git'] = true;
    }

    // Disable colors when redirecting output to file ./tidy.php > file
    if (\function_exists('stream_isatty')) {
        $opt['colors'] = (stream_isatty(STDOUT)) ? $opt['colors'] : false;
    }

    return $opt;
}

/**
 * Script exits if provided checks fail.
 */
function check(array $opt): void
{
    // Check for help
    if ($opt['help']) {
        help();
    }

    // Invalid option
    if ($opt['invalid']) {
        invalid($opt['invalid']);
    }

    // Check if path is set
    if (empty($opt['path'])) {
        invalid('missing path');
    }

    if ($opt['fix'] && !$opt['yes']) {
        prompt();
    }

    return;
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
 * Output short invalid usage error and exit.
 */
function invalid(string $error = 'INVALID USAGE'): void
{
    output('tidy.php: **'.$error.'**', false);
    output("Try '*php tidy.php --help*' for more info.", false);

    exit(1);
}

/**
 * Prompt user to continue executing script or exit.
 */
function prompt(): void
{
    $opt = options();

    output('This will overwrite'.(is_dir($opt['path'])?' all files in':'').' *'.$opt['path'].'*');
    output('Backup copies '.($opt['backup'] ? 'will' : "**won't**")." be created.\n");
    output('Do you want to continue `[yes|no]`?');

    $handle = fopen('php://stdin', 'r');
    $line = fgets($handle);

    if (!in_array(trim((strtolower($line))), ['yes', 'y', '1'])) {
        output('**ABORTING**');

        exit;
    }
}

/**
 * Trim trailing spaces and tabs from each line.
 */
function trimTrailingWhitespace(string $content): string
{
    return preg_replace('/(*BSR_ANYCRLF)[\t ]+(\R|$)/m', '$1', $content);
}

/**
 * Trim spaces and tabs from the beginning of the file.
 */
function trimBeginningWhitespace(string $content): string
{
    if (preg_match('/^[ \t\n\r]+/', $content, $matches)) {
        return preg_replace('/[ \t]+/', '', $matches[0]).ltrim($content);
    }

    return $content;
}

/**
 * Trim all newlines from the beginning of the file.
 */
function trimBeginningNewlines(string $content): string
{
    return ltrim($content, "\r\n");
}

/**
 * Insert one missing final newline at the end of the string, using a prevailing
 * EOL from the given string - LF (\n), CRLF (\r\n) or CR (\r).
 */
function insertFinalNewline(string $content): string
{
    if (!in_array(substr($content, -1), ["\n", "\r"])) {
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

    while ($content !== '') {
        if (substr($content, -2) === "\r\n") {
            $newlines[] = "\r\n";
            $content = substr($content, 0, -2);
            continue;
        }

        if (substr($content, -1) === "\n") {
            $newlines[] = "\n";
            $content = substr($content, 0, -1);
            continue;
        }

        if (substr($content, -1) === "\r") {
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

    for ($i = 0; $i < $max; $i++) {
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
    if ($content === '') {
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
function getEols(string $content): array
{
    $eols = ['lf' => 0, 'crlf' => 0, 'cr' => 0];

    // Match all LF, CRLF and CR EOL characters
    preg_match_all('/(*BSR_ANYCRLF)\R/', $content, $matches);

    foreach ($matches[0] as $match) {
        if ($match === "\n") {
            ++$eols['lf'];
        } elseif ($match === "\r\n") {
            ++$eols['crlf'];
        } elseif ($match === "\r") {
            ++$eols['cr'];
        }
    }

    return $eols;
}

/**
 * Convert all EOL characters to default EOL.
 */
function convertEol(string $content, $file): string
{
    return preg_replace('/(*BSR_ANYCRLF)\R/m', getDefaultEol($file), $content);
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

    if ($file === null || !$opt['has_git']) {
        return $eol;
    }

    if (in_array(relative($file), getCrlfFiles($opt['path']))) {
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
    $files = explode("\0", trim($files));
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
    return preg_replace('/^(\t*)([ ]+)(\t+)/m', '$1$3', $content);
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

    if (file_exists($opt['path'].'/.git') && options()['has_git']) {
        $files = shell_exec(sprintf('cd %s && git ls-tree -r -z HEAD --name-only', escapeshellarg($opt['path'])));
        $files = explode("\0", trim($files));
        $files = preg_filter('/^/', $opt['path'].'/', $files);
    } else {
        $iterator = new \RecursiveDirectoryIterator($opt['path'], \RecursiveDirectoryIterator::SKIP_DOTS);
        $files = iterator_to_array(new \RecursiveIteratorIterator($iterator));
    }

    $blacklist = getBlacklist();
    $whitelist = getWhitelist();

    if (isThisPhpSrc($opt['path'])) {
        $blacklist = array_merge($blacklist, getPhpBlacklist());
        $whitelist = getPhpWhitelist();
    }

    // Remove *.phpt files
    if (!$opt['php_tests'] && !$opt['force_php_tests']) {
        $blacklist = array_merge($blacklist, ['/^.+\.(phpt)$/']);
    }

    // Remove blacklist matches
    foreach ($blacklist as $regex) {
        $data = array_filter($files, function ($item) use ($regex, $opt) {
            $blacklisted = preg_match($regex, relative($item));

            if ($blacklisted && $opt['verbose']) {
                output('*SKIP* '.relative($item).': *ignored*');
            }

            return $blacklisted;
        });

        $files = array_diff($files, $data);
    }

    // Filter files to match the whitelist
    $filtered = [];
    foreach ($whitelist as $regex) {
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
 * Overwrite file and save a backup copy.
 */
function save(string $file, string $content): bool
{
    $opt = options();

    // Backup file
    if ($opt['backup'] && file_put_contents($file.'~', $content) !== false) {
        output('`COPY`  '.relative($file).'~: *backup copy saved*');
    }

    if (file_put_contents($file, $content) === false) {
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
    $fp = fopen($file, 'rb') or error("Cannot open test file: $file");

    $sections = [];
    $section = '';

    while (!feof($fp)) {
        $line = fgets($fp);

        if ($line === false) {
            break;
        }

        if (preg_match('/^--[_A-Z]+--.*/', $line, $matches)) {
            $section = $line;
            $sections[$section] = '';
        } elseif ($section !== '') {
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
 * PHP test files are a special case and need special tidying approach for each
 * section.
 */
function tidyPhpTestFile(string $file): string
{
    $opt = options();
    $logs = [];
    $buffer = [];
    $sections = getTestSections($file);

    if (!preg_match('/^\-\-TEST\-\-/', key($sections))) {
        output('**FAIL**  '.relative($file).': **not a PHP test**');

        return false;
    }

    // Trim trailing whitespace
    if ($opt['trim_trailing_whitespace']) {
        foreach ($sections as $name => $section) {
            $cleanedSection = $section;

            // Trim trailing whitespace after section names
            $cleanedName = trimTrailingWhitespace($name);

            // Trim trailing whitespace for known sections
            if (in_array(trim($cleanedName), [
                '--TEST--',
                '--DESCRIPTION--',
                '--SKIPIF--',
                '--INI--',
                '--ENV--',
                '--EXTENSIONS--',
                '--CREDITS--',
                '--FILE_EXTERNAL--',
                ])
            ) {
                $cleanedSection = trimTrailingWhitespace($cleanedSection);
            }

            // Trim trailing whitespace in certain PHP code lines
            if (in_array(trim($cleanedName), ['--FILE--'])) {
                $cleanedSection = preg_replace(
                    '/(*BSR_ANYCRLF)(\<\?php|\?\>|\)[ ]?\{|\)\;)[\t ]+(\R|$)/m',
                    '$1$2',
                    $cleanedSection);
            }

            if ($cleanedName !== $name || $cleanedSection !== $section) {
                $logs[] = trim($cleanedName).' trailing ws';
            }

            $buffer[$cleanedName] = $cleanedSection;
        }
    }

    return processFile($file, mergeTestSections($sections), mergeTestSections($buffer), $logs);
}

function tidyFile(string $file): bool
{
    $opt = options();

    $original = $cleaned = $buffer = file_get_contents($file);
    $logs = [];

    if ($opt['trim_trailing_whitespace']) {
        $buffer = trimTrailingWhitespace($buffer);
    }

    if ($buffer !== $cleaned) {
        $logs[] = 'trailing whitespace';
        $cleaned = $buffer;
    }

    if ($opt['trim_final_newlines']) {
        $buffer = trimFinalNewlines($buffer, $opt['max_newlines']);
    }

    if ($buffer !== $cleaned) {
        $eols = getFinalNewlines($cleaned);
        $logs[] = count($eols).' final EOL(s)';
        $cleaned = $buffer;
    }

    if ($opt['insert_final_newline']) {
        $buffer = insertFinalNewline($buffer);
    }

    if ($buffer !== $cleaned) {
        $logs[] = 'missing final EOL';
        $cleaned = $buffer;
    }

    if ($opt['eol']) {
        $buffer = convertEol($buffer, $file);
    }

    if ($buffer !== $cleaned) {
        $eols = getEols($cleaned);
        $message = ($eols['lf'] > 0) ? 'LF ' : '';
        $message .= ($eols['crlf'] > 0) ? 'CRLF ' : '';
        $message .= ($eols['cr'] > 0) ? 'CR ' : '';
        $logs[] = $message.'line terminators';
        $cleaned = $buffer;
    }

    if($opt['trim_beginning_whitespace']) {
        $buffer = trimBeginningWhitespace($buffer);
    }

    if ($buffer !== $cleaned) {
        $logs[] = 'beginning whitespace';
        $cleaned = $buffer;
    }

    if($opt['trim_beginning_newlines']) {
        $buffer = trimBeginningNewlines($buffer);
    }

    if ($buffer !== $cleaned) {
        $logs[] = 'beginning newlines';
        $cleaned = $buffer;
    }

    if($opt['clean_space_before_tab']) {
        $buffer = cleanSpaceBeforeTab($buffer);
    }

    if ($buffer !== $cleaned) {
        $logs[] = 'space before tab';
        $cleaned = $buffer;
    }

    return processFile($file, $original, $cleaned, $logs);
}

/**
 * Output log messages and/or save file.
 */
function processFile(string $file, string $original, string $cleaned, array $logs): bool
{
    $opt = options();

    if ($cleaned !== $original) {
        if ($opt['fix']) {
            output('`FIXED` '.relative($file).': `'.implode(', ', $logs).'`');

            return save($file, $cleaned);
        }

        output('**FAIL**  '.relative($file).': **'.implode(', ', $logs).'**');

        return false;
    }

    if ($opt['verbose']) {
        output('`PASS`  '.relative($file));
    }

    return true;
}

function init(array $argv): int
{
    $timeStart = microtime(true);

    $opt = options($argv);
    check($opt);

    output("Executing `tidy.php`\n");
    output('Working tree: '.$opt['path']."\n");

    $exitCode = 0;
    foreach (getFiles() as $file) {
        if (!$opt['force_php_tests'] && preg_match('/^.+\.phpt$/', $file)) {
            $return = tidyPhpTestFile($file);
        } else {
            $return = tidyFile($file);
        }

        $exitCode = ($return && $exitCode === 0) ? 0 : 1;
    }

    $time = round((microtime(true) - $timeStart), 3);

    output('*'.($opt['fix'] ? 'Fixed' : 'Checked').'* all files in '.$time.' sec');

    return $exitCode;
}

exit(init($argv));
