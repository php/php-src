--TEST--
GH-24578 (Function JIT produces an incorrect result in a string-scanning loop)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=64M
opcache.jit=function
--EXTENSIONS--
opcache
--FILE--
<?php
final class Repro {
    public static function skipComment(string $sql, int $position): int {
        $byte = $sql[$position];
        $next = $sql[$position + 1] ?? '';
        if ($byte === '/' && $next === '*') {
            $end = strpos($sql, '*/', $position + 2);
            return $end === false ? strlen($sql) : $end + 2;
        }
        $dashes = $byte === '-' && $next === '-' && strspn($sql[$position + 2] ?? "\n", " \t\r\n") === 1;
        if ($byte !== '#' && !$dashes) {
            return $position;
        }
        $end = strpos($sql, "\n", $position);
        return $end === false ? strlen($sql) : $end + 1;
    }

    public static function skipQuoted(string $sql, int $position): int {
        $quote = $sql[$position];
        $length = strlen($sql);
        $stops = $quote === '`' ? '`' : $quote . '\\';
        $position++;
        while ($position < $length) {
            $position += strcspn($sql, $stops, $position);
            if ($position >= $length) {
                break;
            }
            if ($sql[$position] === '\\') {
                $position += 2;
            } elseif (($sql[$position + 1] ?? '') === $quote) {
                $position += 2;
            } else {
                return $position + 1;
            }
        }
        return $length;
    }

    public static function interpolate(string $sql, array $params): ?string {
        $length = strlen($sql);
        $count = count($params);
        $text = '';
        $used = 0;
        $copied = 0;
        $position = 0;
        while ($position < $length) {
            $position += strcspn($sql, "?'\"`#-/", $position);
            if ($position >= $length) {
                break;
            }
            $byte = $sql[$position];
            if ($byte === '?') {
                $text .= substr($sql, $copied, $position - $copied) . (string) $params[$used];
                $used++;
                $position++;
                $copied = $position;
            } elseif ($byte === "'" || $byte === '"' || $byte === '`') {
                $position = self::skipQuoted($sql, $position);
            } else {
                $after = self::skipComment($sql, $position);
                $position = $after === $position ? $position + 1 : $after;
            }
        }
        return $used === $count ? $text . substr($sql, $copied) : null;
    }
}

var_dump(Repro::interpolate('SELECT ? AS v', [3]));
var_dump(Repro::interpolate('SELECT 5 -', []));
var_dump(Repro::interpolate('SELECT 5 / 2', []));
?>
--EXPECT--
string(13) "SELECT 3 AS v"
string(10) "SELECT 5 -"
string(12) "SELECT 5 / 2"
