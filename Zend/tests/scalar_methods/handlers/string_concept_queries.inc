<?php

/* String API concept: Queries
 *
 * This concept implements the ability to add additional functionality for the
 * indexOf(), contains(), etc methods. Instead of passing a string to these
 * methods a Query object can be passed, which implements a certain behavior for the
 * method. (Note: I don't really like the word "query" here, but I couldn't yet think
 * of how to properly call this.)
 *
 * A few examples of what is possible with this concept:
 *
 *     $str->contains(str\anyOf(['foo', 'bar', 'hello', 'world']))
 *     $str->endsWith(str\noneOf(['.c', '.ho', '.lo']))
 *     $str->indexOf(str\anyOf('0123456789')) // finds first digit in string
 *
 * The str\anyOf() and str\noneOf() functions above return a Query object (like
 * AnyOfStringsQuery) and the method then invokes a method on the query object. E.g.
 * $str->contains($query) is translated to $query->isContainedIn($string).
 *
 * The initial motivation behind this concept was to find a good replacement for the old
 * strspn, strcspn and strpbrk functions without introducing new methods on the string type.
 * This started off as just one Mask class that could be passed to indexOf() and developed
 * into this more general feature.
 *
 * I'm not sure how many more use cases this has apart from the above (and as such I'm not sure
 * how necessary it is).
 */

namespace str;

class HandlerWithQueries extends Handler {
    public function indexOf($string, $offset = 0) {
        if ($string instanceof Query) {
            return $string->firstIndexIn($this);
        } else {
            return parent::indexOf($string, $offset);
        }
    }

    public function lastIndexOf($string, $offset = 0) {
        if ($string instanceof Query) {
            return $string->lastIndexIn($this);
        } else {
            return parent::lastIndexOf($string, $offset);
        }
    }

    public function contains($string) {
        if ($string instanceof Query) {
            return $string->isContainedIn($this);
        } else {
            return parent::contains($string);
        }
    }

    public function startsWith($string) {
        if ($string instanceof Query) {
            return $string->isStartOf($this);
        } else {
            return parent::startsWith($string);
        }
    }

    public function endsWith($string) {
        if ($string instanceof Query) {
            return $string->isEndOf($this);
        } else {
            return parent::endsWith($string);
        }
    }
}

function anyOf($charsOrStrings) {
    if (is_array($charsOrStrings)) {
        return new AnyOfStringsQuery($charsOrStrings);
    } else {
        return new AnyOfCharsQuery($charsOrStrings);
    }
}

function noneOf($charsOrStrings) {
    if (is_array($charsOrStrings)) {
        return new NoneOfStringsQuery($charsOrStrings);
    } else {
        return new NoneOfCharsQuery($charsOrStrings);
    }
}

interface Query {
    public function firstIndexIn($string, $offset = 0);
    public function lastIndexIn($string, $offset = 0);
    public function isContainedIn($string);
    public function isStartOf($string);
    public function isEndOf($string);
}

class AnyOfCharsQuery implements Query {
    protected $mask;

    public function __construct($mask) {
        $this->mask = $mask;
    }

    public function firstIndexIn($string, $offset = 0) {
        $len = strcspn($string, $this->mask, $offset);
        return $len === $string->length() ? false : $len;
    }

    public function lastIndexIn($string, $offset = 0) {
        /* Doesn't look like PHP has native functions for this */
        $reverse = $string->slice($offset)->reverse();
        $len = strcspn($string, $this->mask);
        return $len === $reverse->length() ? false : $string->length() - $len - 1;
    }

    public function isContainedIn($string) {
        return false !== $this->firstIndexIn($string);
    }

    public function isStartOf($string) {
        if (0 === $string->length()) {
            return false;
        }

        return $this->mask->contains($string[0]);
    }

    public function isEndOf($string) {
        if (0 === $string->length()) {
            return false;
        }

        return $this->mask->contains($string[$string->length() - 1]);
    }
}

class NoneOfCharsQuery implements Query {
    protected $mask;

    public function __construct($mask) {
        $this->mask = $mask;
    }

    public function firstIndexIn($string, $offset = 0) {
        $len = strspn($string, $this->mask, $offset);
        return $len === $string->length() ? false : $len;
    }

    public function lastIndexIn($string, $offset = 0) {
        /* Doesn't look like PHP has native functions for this */
        $reverse = $string->slice($offset)->reverse();
        $len = strspn($string, $this->mask);
        return $len === $reverse->length() ? false : $string->length() - $len - 1;
    }

    public function isContainedIn($string) {
        if (0 === $string->length()) {
            return true;
        }

        return false !== $this->firstIndexIn($string);
    }

    public function isStartOf($string) {
        if (0 === $string->length()) {
            return true;
        }

        return !$this->mask->contains($string[0]);
    }

    public function isEndOf($string) {
        if (0 === $string->length()) {
            return true;
        }

        return !$this->mask->contains($string[$string->length() - 1]);
    }
}

class AnyOfStringsQuery implements Query {
    protected $strings;

    public function __construct(array $strings) {
        $this->strings = $strings;
    }

    public function firstIndexIn($string, $offset = 0) {
        $smallestIndex = PHP_MAX_INT;
        foreach ($this->strings as $str) {
            if (false === $index = $string->indexOf($str)) {
                continue;
            }

            if ($index < $smallestIndex) {
                $smallestIndex = $index;
            }
        }

        return $smallestIndex === PHP_MAX_INT ? false : $smallestIndex;
    }

    public function lastIndexIn($string, $offset = 0) {
        $largestIndex = -1;
        foreach ($this->strings as $str) {
            if (false === $index = $string->lastIndexOf($str)) {
                continue;
            }

            if ($index > $largestIndex) {
                $largestIndex = $index;
            }
        }

        return $largestIndex === -1 ? false : $largestIndex;
    }

    public function isContainedIn($string) {
        return $this->checkPredicate($string, 'contains');
    }

    public function isStartOf($string) {
        return $this->checkPredicate($string, 'startsWith');
    }

    public function isEndOf($string) {
        return $this->checkPredicate($string, 'endsWith');
    }

    protected function checkPredicate($string, $methodName) {
        foreach ($this->strings as $str) {
            if ($string->$methodName($str)) {
                return true;
            }
        }

        return false;
    }
}

class NoneOfStringsQuery implements Query {
    protected $strings;

    public function __construct(array $strings) {
        $this->strings = $strings;
    }

    public function firstIndexIn($string, $offset = 0) {
        $indexes = [];
        foreach ($this->strings as $str) {
            if (false !== $index = $string->indexOf($str)) {
                $indexes[$index] = true;
            }
        }

        for ($i = 0, $l = $string->length(); $i < $l; ++$i) {
            if (!isset($indexes[$i])) {
                return $i;
            }
        }

        return false;
    }

    public function lastIndexIn($string, $offset = 0) {
        $indexes = [];
        foreach ($this->strings as $str) {
            if (false !== $index = $string->lastIndexOf($str)) {
                $indexes[$index] = true;
            }
        }

        for ($i = $string->length() - 1; $i >= 0; --$i) {
            if (!isset($indexes[$i])) {
                return $i;
            }
        }

        return false;
    }

    public function isContainedIn($string) {
        return $this->checkPredicate($string, 'contains');
    }

    public function isStartOf($string) {
        return $this->checkPredicate($string, 'startsWith');
    }

    public function isEndOf($string) {
        return $this->checkPredicate($string, 'endsWith');
    }

    protected function checkPredicate($string, $methodName) {
        foreach ($this->strings as $str) {
            if ($string->$methodName($str)) {
                return false;
            }
        }

        return true;
    }
}
