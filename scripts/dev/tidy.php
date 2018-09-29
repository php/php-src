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
  <yellow>NAME</yellow>

    tidy.php - tidy trailing whitespace and end of lines

  <yellow>SYNOPSIS</yellow>

    php tidy.php [OPTION...] input-path

  <yellow>DESCRIPTION</yellow>

    This script fixes common file issues. It can trim trailing spaces and tabs
    from each line, trim multiple final newlines into a single newline, insert
    missing final newline or convert end of line (EOL) characters to a preferred
    style.

    When input-path is a directory, known binary files (images...) are ignored.

    When input-path is a Git repository, files in .gitignore are ignored.

    When input-path is the php-src root directory, it additionally ignores
    certain generated files, bundled libraries, test data files, and works only
    on the predefined whitelist.

    By default, original files are backed up (file.ext~).

    According to POSIX, a line is a sequence of zero or more non-' <newline>'
    characters plus a terminating '<newline>' character. Normal text files
    should usually have at least one final newline character. Newline (EOL)
    characters:
    - <yellow>LF (\\n)</yellow> (*nix and Mac, default)
    - <yellow>CRLF (\\r\\n)</yellow> (Windows)
    - <yellow>CR (\\r)</yellow> (old Mac, obsolete)

    This is a recurring issue in text files and beside the multiple preferred
    coding styles, also certain text editors behave differently. Some fix these
    automatically when saving files, some utilize .editorconfig file and some
    leave files intact.

    Unless tracked specifically, Git stores files in index with LF line endings.
    To see EOLs in the Git repository's index and current working tree:
    <green>git ls-files --eol</green>

    Git provides several configuration options (global or per repository) to
    deal with these issues:
    - <yellow>core.autocrlf = false</yellow> or unspecified
      This is Git default behavior. The <yellow>core.eol</yellow> is the EOL
      character for files in the working tree if there is text attribute set in
      the .gitattributes. Default core.eol is native. When of no attribute text
      is unspecified, text files checked out from the repository keep the
      original EOL in the working tree. Text files with LF or CRLF committed
      back to the repository don't get normalized to LF and are tracked as are.
    - <yellow>core.autocrlf = input</yellow>
      It overwrites core.eol. Text files checked out from the repository keep
      original EOL in the working tree. Text files with CRLF are normalized to
      LF when committed to repository.
    - <yellow>core.autocrlf = true</yellow>
      Text files checked out from the repository are converted to CRLF in
      working tree. When committed back to the repository, CRLF gets normalized
      to LF.

    The .gitattributes relating to EOL:
    - <yellow>eol=lf|crlf</yellow>
    - <yellow>text=auto|set|unset|unspecified</yellow>

    This script uses some of these Git settings to determine the default EOL,
    otherwise EOL can be manually overridden. By default, the LF (\\n) is used.

    Git configuration cheat sheet:

    | core.autocrlf    | core.eol       | text     | eol  | Worktree | Index   |
    |------------------|----------------|----------|------|----------|---------|
    | false,input      | native         |          |      | LF       | LF      |
    | true             | native,lf,crlf |          |      | CRLF     | LF      |
    | true,false,input | native,lf,crlf |          | lf   | LF       | LF      |
    | true,false,input | native,lf,crlf |          | crlf | CRLF     | LF/CRLF |
    | false            | crlf           |          |      | CRLF     | CRLF    |
    | false            | native         | set,auto |      | native   | LF      |
    | true             | native         | set,auto |      | CRLF     | LF      |

    To fix Git repository files and commit EOL changes in the index,
    core.autocrlf=false and core.eol=native need to be set.

    For platform agnostic projects one of the solutions is to commit files with
    LF to index and specify the eol attribute in the .gitattributes for files
    that require a specific and same line endings on all systems (either LF or
    CRLF).

  <yellow>OPTIONS</yellow>

    <green>-h, --help</green>
            Display this help.

    <green>-w, --trim-trailing-whitespace</green>
            Trim trailing whitespace (spaces and tabs).

    <green>-N, --trim-final-newlines [<max>]</green>
            Trim redundant final newlines. If <max> is set, allow maximum of
            <max> trailing final newlines. Default <yellow><max>=1</yellow>

    <green>-n, --insert-final-newline</green>
            Insert a final newline (EOL character) if one is missing at the end
            of the file. Default newline character is LF. If the file has
            multiple different EOL characters (LF, CRLF, or CR), the prevailing
            EOL is used as a final newline. Redundant final newlines or final
            spaces are not trimmed by using only this option.

    <green>-e, --end-of-line [<newline>]</green>
            Fix EOL sequence. *nix and Mac use LF, Windows uses CRLF. The default
            EOL character is determined using Git. If it can't be determined
            the default LF (\\n) is used. It can be manually overridden with the
            <yellow><newline></yellow> value. Can be either <yellow>LF</yellow>
            or <yellow>CRLF</yellow>.

    <green>    --no-colors</green>
            Disable colors in the script output.

    <green>    --no-backup</green>
            Disable backed up files. Use with caution to not overwrite something
            by accident.

    <green>-f, --fix</green>
            Edit and save files in place.

    <green>-q, --quiet</green>
            Do not output any message. This also sets non-interactive mode and
            no questions are asked.

    <green>-y, --yes</green>
            Script won't ask any question and will use an automatic yes to all
            prompts. Used for non-interactive mode.

    <green>-v, --verbose</green>
            Increase the number of output information.

  <yellow>EXAMPLES</yellow>

    Check php-src with all rules enabled:
      <green>scripts/dev/tidy.php .</green>

    Fix php-src with all rules enabled in non-interactive mode without backups:
      <green>scripts/dev/tidy.php -f -y --no-backup .</green>

    Fix custom path or file with all rules enabled:
      <green>./tidy.php --fix input-path</green>

    Trim only trailing whitespace:
      <green>./tidy.php -w --fix input-path</green>

    Trim trailing whitespace, trim and insert missing final newlines:
      <green>./tidy.php -w -n -N -f input-path</green>

    Trim final newlines and allow up to two or less final newlines:
      <green>./tidy.php -N 2 --fix input-path</green>

    Convert newlines to LF style:
      <green>php tidy.php -e LF -f ./input-path</green>

    Convert newlines to CRLF style:
      <green>php tidy.php -e CRLF -f ./input-path</green>

    Check and list of all files script will work on:
      <green>php tidy.php -v ./input-path</green>
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
        '/^\.git\/.+$/',

        // Known binary files based on extension
        '/^.+\.(a|bmp|bz2|data|exe|gd|gd2|gif|gz|ico|iff|jp2|jpc|jpeg|jpg)$/',
        '/^.+\.(la|lo|mo|mov|mp3|o|obj|odt|otf)$/',
        '/^.+\.(patch|pdf|phar|phar\.php|png|ppt|psd)$/',
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
        '/^.+\.(phpt)$/',
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
        '/^.+\.(js|l|m4|macros|md|mk|php|pl|po|re|rss|sh|txt)$/',
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
        'fix'                      => false,
        'trim_trailing_whitespace' => false,
        'insert_final_newline'     => false,
        'trim_final_newlines'      => false,
        'end_of_line'              => false,
        'default_end_of_line'      => null,
        'max_newlines'             => 1,
        'colors'                   => true,
        'backup'                   => true,
        'quiet'                    => false,
        'path'                     => null,
        'help'                     => false,
        'invalid'                  => false,
        'has_git'                  => false,
        'yes'                      => false,
        'verbose'                  => false,
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
            case '--end-of-line':
                if (isset($argv[$i+1])
                    && in_array(strtolower($argv[$i+1]), ['lf', 'crlf'])
                ) {
                    $default = ['lf' => "\n", 'crlf' => "\r\n"];
                    $opt['default_end_of_line'] = $default[strtolower($argv[$i+1])];
                    array_splice($argv, $i, 1);
                }
                $opt['end_of_line'] = true;
            break;
            case '-f':
            case '--fix':
                $opt['fix'] = true;
            break;
            case '--no-colors':
                $opt['colors'] = false;
            break;
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

    // By default enable all fixing rules if none were specified
    if (
        !$opt['trim_trailing_whitespace']
        && !$opt['insert_final_newline']
        && !$opt['trim_final_newlines']
        && !$opt['end_of_line']
    ) {
        $opt['trim_trailing_whitespace'] = true;
        $opt['insert_final_newline'] = true;
        $opt['trim_final_newlines'] = true;
        $opt['end_of_line'] = true;
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
 * Output message based on given options.
 */
function output(?string $message = null, ?bool $quiet = null, ?bool $colors = null): void
{
    $opt = options();

    if ($quiet ?? $opt['quiet']) {
        return;
    }

    $colors = $colors ?? $opt['colors'];
    $red = ($colors) ? "\e[41m" : '';
    $green = ($colors) ? "\e[32m" : '';
    $yellow = ($colors) ? "\e[33m" : '';
    $default = ($colors) ? "\e[0m" : '';

    $starts = ['<red>', '<green>', '<yellow>'];
    $replacements = [$red, $green, $yellow];
    $message = str_replace($starts, $replacements, $message);

    $ends = ['</red>', '</green>', '</yellow>'];
    $message = str_replace($ends, $default, $message);

    echo $message."\n";
}

/**
 * Output short invalid usage error and exit.
 */
function invalid(string $error = 'INVALID USAGE'): void
{
    output("tidy.php: <red>$error</red>", false);
    output("Try '<yellow>php tidy.php --help</yellow>' for more info.", false);

    exit(1);
}

/**
 * Prompt user to continue executing script or exit.
 */
function prompt(): void
{
    $opt = options();

    output('This will fix all <yellow>'.$opt['path'].'</yellow> files');

    if ($opt['backup']) {
        output("Backed up copies will be created.\n");
    }

    output('Do you want to continue <green>[yes|no]</green>?');

    $handle = fopen('php://stdin', 'r');
    $line = fgets($handle);

    if (!in_array(trim((strtolower($line))), ['yes', 'y', '1'])) {
        output('<red>ABORTING</red>');

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
 * If there is missing a final newline at the end of the string, append one
 * prevailing EOL from the given content - LF (\n), CRLF (\r\n) or CR (\r).
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
 * Get the default EOL character by checking a manually provided command line
 * option -e|--end-of-line <newline>, otherwise use Git configuration. If input
 * path is not Git repository or Git is not present the default LF (\n) is used.
 */
function getDefaultEol(?string $file = null): string
{
    static $eol;

    if (!empty($eol) && $file === null) {
        return $eol;
    }

    $opt = options();

    if (!empty($opt['default_end_of_line'])) {
        $eol = $opt['default_end_of_line'];

        return $eol;
    }

    // Default EOL is *nix style LF (\n) character
    $eol = "\n";

    if (!$opt['has_git']) {
        return $eol;
    }

    if (!empty($file)) {
        $entries = getListOfCrlfFiles();

        foreach ($entries as $entry) {
            $items = preg_split('/\s+/', $entry);
            $eolInIndex = $items[0];
            $eolAttribute = $items[2];
            $fileName = $items[3];

            if ($eolInIndex === 'i/crlf' && preg_match('/^attr\/.*eol=crlf.*$/', $eolAttribute)) {
                return "\r\n";
            }
        }
    }

    return $eol;
}

/**
 * Files with eol=crlf attribute set should have CRLF line endings. All others
 * are converted to LF.
 */
function getListOfCrlfFiles(): array
{
    static $entries = [];

    if (!empty($entries)) {
        return $entries;
    }

    $opt = options();

    $entries = shell_exec(sprintf('cd %s && git ls-files -z --eol 2>&1', escapeshellarg($opt['path'])));
    $entries = explode("\0", trim($entries));

    $entries = array_filter($entries, function ($item) {
        return preg_match('/^i\/crlf.*$/', $item);
    });

    return $entries;
}

/**
 * Convert all mismatched EOL characters to default LF (\n). If working in a Git
 * repository path and Git is available, Git determines the EOL.
 */
function convertEol(string $content, $file): string
{
    return preg_replace('/(*BSR_ANYCRLF)\R/m', getDefaultEol($file), $content);
}

/**
 * Get filtered files from a given path. Files in .gitignore are omitted.
 */
function getFiles(string $path): array
{
    if (is_file($path)) {
        return [$path];
    }

    if (file_exists($path.'/.git') && options()['has_git']) {
        $files = shell_exec(sprintf('cd %s && git ls-tree -r -z HEAD --name-only', escapeshellarg($path)));
        $files = explode("\0", trim($files));
        $files = preg_filter('/^/', $path.'/', $files);
    } else {
        $innerIterator = new \RecursiveDirectoryIterator(
            $path,
            \RecursiveDirectoryIterator::SKIP_DOTS
        );
        $iterator = new RecursiveIteratorIterator($innerIterator);

        $files = iterator_to_array($iterator);
    }

    $blacklist = getBlacklist();
    $whitelist = getWhitelist();

    if (isThisPhpSrc($path)) {
        $blacklist = array_merge($blacklist, getPhpBlacklist());
        $whitelist = getPhpWhitelist();
    }

    // Remove blacklist matches
    foreach ($blacklist as $regex) {
        $data = array_filter($files, function ($item) use ($regex, $path) {
            $pathnameWithoutBase = str_replace($path.'/', '', $item);
            return preg_match($regex, $pathnameWithoutBase);
        });

        $files = array_diff($files, $data);
    }

    // Filter files to match the whitelist
    $filtered = [];
    foreach ($whitelist as $regex) {
        $data = array_filter($files, function ($item) use ($regex, $path) {
            $pathnameWithoutBase = str_replace($path.'/', '', $item);
            return preg_match($regex, $pathnameWithoutBase);
        });

        $filtered = array_merge($filtered, array_diff($data, $filtered));
    }

    return $filtered;
}

/**
 * Overwrite file and save a backup copy.
 */
function save(string $file, string $content): bool
{
    if (file_put_contents($file, $content) === false) {
        output($file.': <red>Error: file could not be saved</red>');

        return false;
    }

    // Backup file
    if (options()['backup'] && file_put_contents($file.'~', $content) !== false) {
        output($file.'~: <yellow>backup copy saved</yellow>');
    }

    return true;
}

/**
 * Tidies given file according to the provided rules.
 */
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

    if ($opt['end_of_line']) {
        $buffer = convertEol($buffer, $file);
    }

    if ($buffer !== $cleaned) {
        $eols = getEols($cleaned);

        $message = ($eols['lf'] > 0) ? 'LF ' : '';
        $message .= ($eols['crlf'] > 0) ? 'CRLF ' : '';
        $message .= ($eols['cr'] > 0) ? 'CR ' : '';

        if ($message !== '') {
            $logs[] = $message.'line terminators';
        }

        $cleaned = $buffer;
    }

    // Output message and fix file
    if ($cleaned !== $original) {
        if ($opt['fix']) {
            output($file.': <green>'.implode(', ', $logs).'</green>');

            return save($file, $cleaned, $opt);
        }

        output($file.': <red>'.implode(', ', $logs).'</red>');

        return false;
    }

    if ($opt['verbose']) {
        output($file.': <green>OK</green>');
    }

    return true;
}

function init(array $argv): int
{
    $opt = options($argv);
    check($opt);

    output("Executing <green>tidy.php</green>\n");

    $exitCode = 0;
    foreach (getFiles($opt['path']) as $file) {
        $exitCode = (tidyFile($file) && $exitCode === 0) ? 0 : 1;
    }

    output('<yellow>All done!</yellow>');

    return $exitCode;
}

exit(init($argv));
