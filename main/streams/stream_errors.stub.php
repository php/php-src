<?php

/**
 * @generate-class-entries static
 */
class StreamException extends Exception
{
    protected string $wrapperName;

    protected ?string $param = null;

    public function getParam(): ?string {}

    public function getWrapperName(): string {}

}