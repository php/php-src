<?php

/** @generate-class-entries */

namespace Uri {
    /** @strict-properties */
    class UriException extends \Exception
    {
    }

    /** @strict-properties */
    class InvalidUriException extends \Uri\UriException
    {
    }
}

namespace Uri\WhatWg {
    /** @strict-properties */
    class InvalidUrlException extends \Uri\InvalidUriException
    {
        public readonly array $errors;
    }
}
