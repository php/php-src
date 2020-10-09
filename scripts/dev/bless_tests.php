#!/usr/bin/env php
<?php

if ($argc < 2) {
    die("Usage: php bless_tests.php dir/\n");
}

$files = getFiles(array_slice($argv, 1));
foreach ($files as $path) {
    if (!preg_match('/^(.*)\.phpt$/', $path, $matches)) {
        // Not a phpt test
        continue;
    }

    $outPath = $matches[1] . '.out';
    if (!file_exists($outPath)) {
        // Test did not fail
        continue;
    }

    $phpt = file_get_contents($path);
    $out = file_get_contents($outPath);

    if (false !== strpos($phpt, '--XFAIL--')) {
        // Don't modify expected output of XFAIL tests
        continue;
    }

    // Don't update EXPECTREGEX tests
    if (!preg_match('/--EXPECT(F?)--(.*)$/s', $phpt, $matches)) {
        continue;
    }

    $oldExpect = trim($matches[2]);
    $isFormat = $matches[1] == 'F';
    if ($isFormat) {
        $out = generateMinimallyDifferingOutput($out, $oldExpect);
    } else {
        $out = normalizeOutput($out);
    }

    $phpt = insertOutput($phpt, $out);
    file_put_contents($path, $phpt);
}

function getFiles(array $dirsOrFiles): \Iterator {
    foreach ($dirsOrFiles as $dirOrFile) {
        if (is_dir($dirOrFile)) {
            $it = new RecursiveIteratorIterator(
                new RecursiveDirectoryIterator($dirOrFile),
                RecursiveIteratorIterator::LEAVES_ONLY
            );
            foreach ($it as $file) {
                yield $file->getPathName();
            }
        } else if (is_file($dirOrFile)) {
            yield $dirOrFile;
        } else {
            die("$dirOrFile is not a directory or file\n");
        }
    }
}

function normalizeOutput(string $out): string {
    $out = preg_replace('/in (\/|[A-Z]:\\\\).+ on line \d+$/m', 'in %s on line %d', $out);
    $out = preg_replace('/in (\/|[A-Z]:\\\\).+:\d+$/m', 'in %s:%d', $out);
    $out = preg_replace('/^#(\d+) (\/|[A-Z]:\\\\).+\(\d+\):/m', '#$1 %s(%d):', $out);
    $out = preg_replace('/Resource id #\d+/', 'Resource id #%d', $out);
    $out = preg_replace('/resource\(\d+\) of type/', 'resource(%d) of type', $out);
    $out = preg_replace(
        '/Resource ID#\d+ used as offset, casting to integer \(\d+\)/',
        'Resource ID#%d used as offset, casting to integer (%d)',
        $out);
    $out = preg_replace('/string\(\d+\) "([^"]*%d)/', 'string(%d) "$1', $out);
    return $out;
}

function formatToRegex(string $format): string {
    $result = preg_quote($format, '/');
    $result = str_replace('%e', '\\' . DIRECTORY_SEPARATOR, $result);
    $result = str_replace('%s', '[^\r\n]+', $result);
    $result = str_replace('%S', '[^\r\n]*', $result);
    $result = str_replace('%w', '\s*', $result);
    $result = str_replace('%i', '[+-]?\d+', $result);
    $result = str_replace('%d', '\d+', $result);
    $result = str_replace('%x', '[0-9a-fA-F]+', $result);
    $result = str_replace('%f', '[+-]?\.?\d+\.?\d*(?:[Ee][+-]?\d+)?', $result);
    $result = str_replace('%c', '.', $result);
    return "/^$result$/s";
}

function generateMinimallyDifferingOutput(string $out, string $oldExpect) {
    $outLines = explode("\n", $out);
    $oldExpectLines = explode("\n", $oldExpect);
    $differ = new Differ(function($oldExpect, $new) {
        if (strpos($oldExpect, '%') === false) {
            return $oldExpect === $new;
        }
        return preg_match(formatToRegex($oldExpect), $new);
    });
    $diff = $differ->diff($oldExpectLines, $outLines);

    $result = [];
    foreach ($diff as $elem) {
        if ($elem->type == DiffElem::TYPE_KEEP) {
            $result[] = $elem->old;
        } else if ($elem->type == DiffElem::TYPE_ADD) {
            $result[] = normalizeOutput($elem->new);
        }
    }
    return implode("\n", $result);
}

function insertOutput(string $phpt, string $out): string {
    return preg_replace_callback('/--EXPECTF?--.*?(--CLEAN--|$)/sD', function($matches) use($out) {
        $hasWildcard = preg_match('/%[resSaAwidxfc]/', $out);
        $F = $hasWildcard ? 'F' : '';
        return "--EXPECT$F--\n" . $out . "\n" . $matches[1];
    }, $phpt);
}

/**
 * Implementation of the the Myers diff algorithm.
 *
 * Myers, Eugene W. "An O (ND) difference algorithm and its variations."
 * Algorithmica 1.1 (1986): 251-266.
 */

class DiffElem
{
    const TYPE_KEEP = 0;
    const TYPE_REMOVE = 1;
    const TYPE_ADD = 2;

    /** @var int One of the TYPE_* constants */
    public $type;
    /** @var mixed Is null for add operations */
    public $old;
    /** @var mixed Is null for remove operations */
    public $new;

    public function __construct(int $type, $old, $new) {
        $this->type = $type;
        $this->old = $old;
        $this->new = $new;
    }
}

class Differ
{
    private $isEqual;

    /**
     * Create differ over the given equality relation.
     *
     * @param callable $isEqual Equality relation with signature function($a, $b) : bool
     */
    public function __construct(callable $isEqual) {
        $this->isEqual = $isEqual;
    }

    /**
     * Calculate diff (edit script) from $old to $new.
     *
     * @param array $old Original array
     * @param array $new New array
     *
     * @return DiffElem[] Diff (edit script)
     */
    public function diff(array $old, array $new) {
        list($trace, $x, $y) = $this->calculateTrace($old, $new);
        return $this->extractDiff($trace, $x, $y, $old, $new);
    }

    private function calculateTrace(array $a, array $b) {
        $n = \count($a);
        $m = \count($b);
        $max = $n + $m;
        $v = [1 => 0];
        $trace = [];
        for ($d = 0; $d <= $max; $d++) {
            $trace[] = $v;
            for ($k = -$d; $k <= $d; $k += 2) {
                if ($k === -$d || ($k !== $d && $v[$k-1] < $v[$k+1])) {
                    $x = $v[$k+1];
                } else {
                    $x = $v[$k-1] + 1;
                }

                $y = $x - $k;
                while ($x < $n && $y < $m && ($this->isEqual)($a[$x], $b[$y])) {
                    $x++;
                    $y++;
                }

                $v[$k] = $x;
                if ($x >= $n && $y >= $m) {
                    return [$trace, $x, $y];
                }
            }
        }
        throw new \Exception('Should not happen');
    }

    private function extractDiff(array $trace, int $x, int $y, array $a, array $b) {
        $result = [];
        for ($d = \count($trace) - 1; $d >= 0; $d--) {
            $v = $trace[$d];
            $k = $x - $y;

            if ($k === -$d || ($k !== $d && $v[$k-1] < $v[$k+1])) {
                $prevK = $k + 1;
            } else {
                $prevK = $k - 1;
            }

            $prevX = $v[$prevK];
            $prevY = $prevX - $prevK;

            while ($x > $prevX && $y > $prevY) {
                $result[] = new DiffElem(DiffElem::TYPE_KEEP, $a[$x-1], $b[$y-1]);
                $x--;
                $y--;
            }

            if ($d === 0) {
                break;
            }

            while ($x > $prevX) {
                $result[] = new DiffElem(DiffElem::TYPE_REMOVE, $a[$x-1], null);
                $x--;
            }

            while ($y > $prevY) {
                $result[] = new DiffElem(DiffElem::TYPE_ADD, null, $b[$y-1]);
                $y--;
            }
        }
        return array_reverse($result);
    }
}
