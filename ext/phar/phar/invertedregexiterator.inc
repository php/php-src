<?php

/** @file invertedregexiterator.inc
 * @ingroup Phar
 * @brief class InvertedRegexIterator
 * @author  Marcus Boerger
 * @date    2007 - 2008
 *
 * Inverted RegexIterator
 */

/** @ingroup Phar
 * @brief   Inverted RegexIterator
 * @author  Marcus Boerger
 * @version 1.0
 */
class InvertedRegexIterator extends RegexIterator
{
    /** @return !RegexIterator::accept()
     */
    function accept()
    {
        return !RegexIterator::accept();
    }
}

?>
