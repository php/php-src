<?php

namespace str;

use extension string str\Handler;

class Handler {
    public static function length($self) {
        return strlen($self);
    }

    /*
     * Slicing methods
     */

    public static function slice($self, $offset, $length = null) {
        $offset = $self->prepareOffset($offset);
        $length = $self->prepareLength($offset, $length);

        if (0 === $length) {
            return '';
        }

        return substr($self, $offset, $length);
    }

    public static function replaceSlice($self, $replacement, $offset, $length = null) {
        $offset = $self->prepareOffset($offset);
        $length = $self->prepareLength($offset, $length);

        return substr_replace($self, $replacement, $offset, $length);
    }

    /*
     * Search methods
     */

    public static function indexOf($self, $string, $offset = 0) {
        $offset = $self->prepareOffset($offset);

        if ('' === $string) {
            return $offset;
        }

        return strpos($self, $string, $offset);
    }

    public static function lastIndexOf($self, $string, $offset = null) {
        if (null === $offset) {
            $offset = $self->length();
        } else {
            $offset = $self->prepareOffset($offset);
        }

        if ('' === $string) {
            return $offset;
        }

        /* Converts $offset to a negative offset as strrpos has a different
         * behavior for positive offsets. */
        return strrpos($self, $string, $offset - $self->length());
    }

    public static function contains($self, $string) {
        return false !== $self->indexOf($string);
    }

    public static function startsWith($self, $string) {
        return 0 === $self->indexOf($string);
    }

    public static function endsWith($self, $string) {
        return $self->lastIndexOf($string) === $self->length() - $string->length();
    }

    public static function count($self, $string, $offset = 0, $length = null) {
        $offset = $self->prepareOffset($offset);
        $length = $self->prepareLength($offset, $length);

        if ('' === $string) {
            return $length + 1;
        }

        return substr_count($self, $string, $offset, $length);
    }

    /* This static function has two prototypes:
     *
     * replace(array(string $from => string $to) $replacements, int $limit = PHP_MAX_INT)
     * replace(string $from, string $to, int $limit = PHP_MAX_INT)
     */
    public static function replace($self, $from, $to = null, $limit = null) {
        if (is_array($from)) {
            $replacements = $from;
            $limit = $to;

            $self->verifyNotContainsEmptyString(
                array_keys($replacements), 'Replacement array keys'
            );

            // strtr() with an empty replacements array will crash in some PHP versions
            if (empty($replacements)) {
                return $self;
            }

            if (null === $limit) {
                return strtr($self, $from);
            } else {
                $self->verifyPositive($limit, 'Limit');
                return self::replaceWithLimit($self, $replacements, $limit);
            }
        } else {
            $self->verifyNotEmptyString($from, 'From string');

            if (null === $limit) {
                return str_replace($from, $to, $self);
            } else {
                $self->verifyPositive($limit, 'Limit');
                return self::replaceWithLimit($self, [$from => $to], $limit);
            }
        }
    }

    public static function split($self, $separator, $limit = PHP_INT_MAX) {
        return explode($separator, $self, $limit);
    }

    public static function chunk($self, $chunkLength = 1) {
        $self->verifyPositive($chunkLength, 'Chunk length');
        return str_split($self, $chunkLength);
    }

    public static function repeat($self, $times) {
        $self->verifyNotNegative($times, 'Number of repetitions');
        return str_repeat($self, $times);
    }

    public static function reverse($self) {
        return strrev($self);
    }

    public static function toLower($self) {
        return strtolower($self);
    }

    public static function toUpper($self) {
        return strtoupper($self);
    }

    public static function trim($self, $characters = " \t\n\r\v\0") {
        return trim($self, $characters);
    }

    public static function trimLeft($self, $characters = " \t\n\r\v\0") {
        return ltrim($self, $characters);
    }

    public static function trimRight($self, $characters = " \t\n\r\v\0") {
        return rtrim($self, $characters);
    }

    public static function padLeft($self, $length, $padString = " ") {
        return str_pad($self, $length, $padString, STR_PAD_LEFT);
    }

    public static function padRight($self, $length, $padString = " ") {
        return str_pad($self, $length, $padString, STR_PAD_RIGHT);
    }

    public static function duplicate($self) {
	return $self . $self;
    }

    protected static function prepareOffset($self, $offset) {
        $len = $self->length();
        if ($offset < -$len || $offset > $len) {
            throw new \InvalidArgumentException('Offset must be in range [-len, len]');
        }

        if ($offset < 0) {
            $offset += $len;
        }

        return $offset;
    }

    protected static function prepareLength($self, $offset, $length) {
        if (null === $length) {
            return $self->length() - $offset;
        }

        if ($length < 0) {
            $length += $self->length() - $offset;

            if ($length < 0) {
                throw new \InvalidArgumentException('Length too small');
            }
        } else {
            if ($offset + $length > $self->length()) {
                throw new \InvalidArgumentException('Length too large');
            }
        }

        return $length;
    }

    protected static function verifyPositive($self, $value, $name) {
        if ($value <= 0) {
            throw new \InvalidArgumentException("$name has to be positive");
        }
    }

    protected static function verifyNotNegative($self, $value, $name) {
        if ($value < 0) {
            throw new \InvalidArgumentException("$name can not be negative");
        }
    }

    protected static function verifyNotEmptyString($self, $value, $name) {
        if ((string) $value === '') {
            throw new \InvalidArgumentException("$name can not be an empty string");
        }
    }

    protected static function verifyNotContainsEmptyString($self, array $array, $name) {
        foreach ($array as $value) {
            if ((string) $value === '') {
                throw new \InvalidArgumentException("$name can not contain an empty string");
            }
        }
    }

    /* This effectively implements strtr with a limit */
    protected static function replaceWithLimit($self, array $replacements, $limit) {
        if (empty($replacements)) {
            return $self;
        }

        self::sortKeysByStringLength($replacements);
        $regex = self::createFromStringRegex($replacements);

        return preg_replace_callback($regex, function($matches) use($replacements) {
            return $replacements[$matches[0]];
        }, $self, $limit);
    }

    protected static function sortKeysByStringLength(array &$array) {
        uksort($array, function($str1, $str2) {
            return $str2->length() - $str1->length();
        });
    }

    protected static function createFromStringRegex(array $replacements) {
        $fromRegexes = [];
        foreach ($replacements as $from => $_) {
            $fromRegexes[] = preg_quote($from, '~');
        }

        return '~(?:' . implode('|', $fromRegexes) . ')~S';
    }
}
