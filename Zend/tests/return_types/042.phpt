--TEST--
__debugInfo can declare union return type
--FILE--
<?php
class UnionType {
    public function __debugInfo(): array|null {}
}

class UnionType2 {
    public function __debugInfo(): null|array {}
}

class UnionTypeOldStyle {
    public function __debugInfo(): ?array {}
}

class JustAnArray {
    public function __debugInfo(): array {}
}

echo 'No problems!';
?>
--EXPECTF--
Deprecated: Returning null from UnionType::__debugInfo() is deprecated, make the return type non-nullable and return an empty array instead in %s on line %d

Deprecated: Returning null from UnionType2::__debugInfo() is deprecated, make the return type non-nullable and return an empty array instead in %s on line %d

Deprecated: Returning null from UnionTypeOldStyle::__debugInfo() is deprecated, make the return type non-nullable and return an empty array instead in %s on line %d
No problems!
