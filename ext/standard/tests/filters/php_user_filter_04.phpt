--TEST--
php_user_filter with invalid seek signature
--FILE--
<?php

class InvalidSeekFilter extends php_user_filter
{
    public function filter($in, $out, &$consumed, bool $closing): int
    {
        return PSFS_PASS_ON;
    }
    
    public function onCreate(): bool
    {
        return true;
    }
    
    public function onClose(): void {}
    
    public function seek($offset): bool
    {
        return true;
    }
}

?>
--EXPECTF--
Fatal error: Declaration of InvalidSeekFilter::seek($offset): bool must be compatible with php_user_filter::seek(int $offset, int $whence): bool in %s on line %d
