<?php

/** @generate-class-entries */

class LogicException extends Exception
{
}

class BadFunctionCallException extends LogicException
{
}

class BadMethodCallException extends BadFunctionCallException
{
}

class DomainException extends LogicException
{
}

class InvalidArgumentException extends LogicException
{
}

class LengthException extends LogicException
{
}

class OutOfRangeException extends LogicException
{
}

class RuntimeException extends Exception
{
}

class OutOfBoundsException extends RuntimeException
{
}

class OverflowException extends RuntimeException
{
}

class RangeException extends RuntimeException
{
}

class UnderflowException extends RuntimeException
{
}

class UnexpectedValueException extends RuntimeException
{
}
