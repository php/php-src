#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\ConstExprEvaluator;
use PhpParser\Node;
use PhpParser\Node\AttributeGroup;
use PhpParser\Node\Expr;
use PhpParser\Node\Name;
use PhpParser\Node\Stmt;
use PhpParser\Node\Stmt\Class_;
use PhpParser\Node\Stmt\Enum_;
use PhpParser\Node\Stmt\Interface_;
use PhpParser\Node\Stmt\Trait_;
use PhpParser\PrettyPrinter\Standard;
use PhpParser\PrettyPrinterAbstract;

error_reporting(E_ALL);
ini_set("precision", "-1");

const PHP_70_VERSION_ID = 70000;
const PHP_80_VERSION_ID = 80000;
const PHP_81_VERSION_ID = 80100;
const PHP_82_VERSION_ID = 80200;
const PHP_83_VERSION_ID = 80300;
const ALL_PHP_VERSION_IDS = [PHP_70_VERSION_ID, PHP_80_VERSION_ID, PHP_81_VERSION_ID, PHP_82_VERSION_ID, PHP_83_VERSION_ID];

/**
 * @return FileInfo[]
 */
function processDirectory(string $dir, Context $context): array {
    $pathNames = [];
    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            $pathNames[] = $pathName;
        }
    }

    // Make sure stub files are processed in a predictable, system-independent order.
    sort($pathNames);

    $fileInfos = [];
    foreach ($pathNames as $pathName) {
        $fileInfo = processStubFile($pathName, $context);
        if ($fileInfo) {
            $fileInfos[] = $fileInfo;
        }
    }
    return $fileInfos;
}

function processStubFile(string $stubFile, Context $context, bool $includeOnly = false): ?FileInfo {
    try {
        if (!file_exists($stubFile)) {
            throw new Exception("File $stubFile does not exist");
        }

        if (!$includeOnly) {
            $stubFilenameWithoutExtension = str_replace(".stub.php", "", $stubFile);
            $arginfoFile = "{$stubFilenameWithoutExtension}_arginfo.h";
            $legacyFile = "{$stubFilenameWithoutExtension}_legacy_arginfo.h";

            $stubCode = file_get_contents($stubFile);
            $stubHash = computeStubHash($stubCode);
            $oldStubHash = extractStubHash($arginfoFile);
            if ($stubHash === $oldStubHash && !$context->forceParse) {
                /* Stub file did not change, do not regenerate. */
                return null;
            }
        }

        if (!$fileInfo = $context->parsedFiles[$stubFile] ?? null) {
            initPhpParser();
            $fileInfo = parseStubFile($stubCode ?? file_get_contents($stubFile));
            $context->parsedFiles[$stubFile] = $fileInfo;

            foreach ($fileInfo->dependencies as $dependency) {
                // TODO add header search path for extensions?
                $prefixes = [dirname($stubFile) . "/", dirname(__DIR__) . "/"];
                foreach ($prefixes as $prefix) {
                    $depFile = $prefix . $dependency;
                    if (file_exists($depFile)) {
                        break;
                    }
                    $depFile = null;
                }
                if (!$depFile) {
                    throw new Exception("File $stubFile includes a file $dependency which does not exist");
                }
                processStubFile($depFile, $context, true);
            }

            $constInfos = $fileInfo->getAllConstInfos();
            $context->allConstInfos = array_merge($context->allConstInfos, $constInfos);
        }

        if ($includeOnly) {
            return $fileInfo;
        }

        $arginfoCode = generateArgInfoCode(
            basename($stubFilenameWithoutExtension),
            $fileInfo,
            $context->allConstInfos,
            $stubHash
        );
        if (($context->forceRegeneration || $stubHash !== $oldStubHash) && file_put_contents($arginfoFile, $arginfoCode)) {
            echo "Saved $arginfoFile\n";
        }

        if ($fileInfo->generateLegacyArginfoForPhpVersionId !== null && $fileInfo->generateLegacyArginfoForPhpVersionId < PHP_80_VERSION_ID) {
            $legacyFileInfo = clone $fileInfo;

            foreach ($legacyFileInfo->getAllFuncInfos() as $funcInfo) {
                $funcInfo->discardInfoForOldPhpVersions();
            }
            foreach ($legacyFileInfo->getAllConstInfos() as $constInfo) {
                $constInfo->discardInfoForOldPhpVersions();
            }
            foreach ($legacyFileInfo->getAllPropertyInfos() as $propertyInfo) {
                $propertyInfo->discardInfoForOldPhpVersions();
            }

            $arginfoCode = generateArgInfoCode(
                basename($stubFilenameWithoutExtension),
                $legacyFileInfo,
                $context->allConstInfos,
                $stubHash
            );
            if (($context->forceRegeneration || $stubHash !== $oldStubHash) && file_put_contents($legacyFile, $arginfoCode)) {
                echo "Saved $legacyFile\n";
            }
        }

        return $fileInfo;
    } catch (Exception $e) {
        echo "In $stubFile:\n{$e->getMessage()}\n";
        exit(1);
    }
}

function computeStubHash(string $stubCode): string {
    return sha1(str_replace("\r\n", "\n", $stubCode));
}

function extractStubHash(string $arginfoFile): ?string {
    if (!file_exists($arginfoFile)) {
        return null;
    }

    $arginfoCode = file_get_contents($arginfoFile);
    if (!preg_match('/\* Stub hash: ([0-9a-f]+) \*/', $arginfoCode, $matches)) {
        return null;
    }

    return $matches[1];
}

class Context {
    public bool $forceParse = false;
    public bool $forceRegeneration = false;
    /** @var array<string, ConstInfo> */
    public array $allConstInfos = [];
    /** @var FileInfo[] */
    public array $parsedFiles = [];
}

class ArrayType extends SimpleType {
    public Type $keyType;
    public Type $valueType;

    public static function createGenericArray(): self
    {
        return new ArrayType(Type::fromString("int|string"), Type::fromString("mixed|ref"));
    }

    public function __construct(Type $keyType, Type $valueType)
    {
        parent::__construct("array", true);

        $this->keyType = $keyType;
        $this->valueType = $valueType;
    }

    public function toOptimizerTypeMask(): string {
        $typeMasks = [
            parent::toOptimizerTypeMask(),
            $this->keyType->toOptimizerTypeMaskForArrayKey(),
            $this->valueType->toOptimizerTypeMaskForArrayValue(),
        ];

        return implode("|", $typeMasks);
    }

    public function equals(SimpleType $other): bool {
        if (!parent::equals($other)) {
            return false;
        }

        assert(get_class($other) === self::class);

        return Type::equals($this->keyType, $other->keyType) &&
            Type::equals($this->valueType, $other->valueType);
    }
}

class SimpleType {
    public string $name;
    public bool $isBuiltin;

    public static function fromNode(Node $node): SimpleType {
        if ($node instanceof Node\Name) {
            if ($node->toLowerString() === 'static') {
                // PHP internally considers "static" a builtin type.
                return new SimpleType($node->toLowerString(), true);
            }

            if ($node->toLowerString() === 'self') {
                throw new Exception('The exact class name must be used instead of "self"');
            }

            assert($node->isFullyQualified());
            return new SimpleType($node->toString(), false);
        }

        if ($node instanceof Node\Identifier) {
            if ($node->toLowerString() === 'array') {
                return ArrayType::createGenericArray();
            }

            return new SimpleType($node->toLowerString(), true);
        }

        throw new Exception("Unexpected node type");
    }

    public static function fromString(string $typeString): SimpleType
    {
        switch (strtolower($typeString)) {
            case "void":
            case "null":
            case "false":
            case "true":
            case "bool":
            case "int":
            case "float":
            case "string":
            case "callable":
            case "object":
            case "resource":
            case "mixed":
            case "static":
            case "never":
            case "ref":
                return new SimpleType(strtolower($typeString), true);
            case "array":
                return ArrayType::createGenericArray();
            case "self":
                throw new Exception('The exact class name must be used instead of "self"');
            case "iterable":
                throw new Exception('This should not happen');
        }

        $matches = [];
        $isArray = preg_match("/(.*)\s*\[\s*\]/", $typeString, $matches);
        if ($isArray) {
            return new ArrayType(Type::fromString("int"), Type::fromString($matches[1]));
        }

        $matches = [];
        $isArray = preg_match("/array\s*<\s*([A-Za-z0-9_|-]+)?(\s*,\s*)?([A-Za-z0-9_|-]+)?\s*>/i", $typeString, $matches);
        if ($isArray) {
            if (empty($matches[1]) || empty($matches[3])) {
                throw new Exception("array<> type hint must have both a key and a value");
            }

            return new ArrayType(Type::fromString($matches[1]), Type::fromString($matches[3]));
        }

        return new SimpleType($typeString, false);
    }

    /**
     * @param mixed $value
     */
    public static function fromValue($value): SimpleType
    {
        switch (gettype($value)) {
            case "NULL":
                return SimpleType::null();
            case "boolean":
                return SimpleType::bool();
            case "integer":
                return SimpleType::int();
            case "double":
                return SimpleType::float();
            case "string":
                return SimpleType::string();
            case "array":
                return SimpleType::array();
            case "object":
                return SimpleType::object();
            default:
                throw new Exception("Type \"" . gettype($value) . "\" cannot be inferred based on value");
        }
    }

    public static function null(): SimpleType
    {
        return new SimpleType("null", true);
    }

    public static function bool(): SimpleType
    {
        return new SimpleType("bool", true);
    }

    public static function int(): SimpleType
    {
        return new SimpleType("int", true);
    }

    public static function float(): SimpleType
    {
        return new SimpleType("float", true);
    }

    public static function string(): SimpleType
    {
        return new SimpleType("string", true);
    }

    public static function array(): SimpleType
    {
        return new SimpleType("array", true);
    }

    public static function object(): SimpleType
    {
        return new SimpleType("object", true);
    }

    public static function void(): SimpleType
    {
        return new SimpleType("void", true);
    }

    protected function __construct(string $name, bool $isBuiltin) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
    }

    public function isScalar(): bool {
        return $this->isBuiltin && in_array($this->name, ["null", "false", "true", "bool", "int", "float"], true);
    }

    public function isNull(): bool {
        return $this->isBuiltin && $this->name === 'null';
    }

    public function isBool(): bool {
        return $this->isBuiltin && $this->name === 'bool';
    }

    public function isInt(): bool {
        return $this->isBuiltin && $this->name === 'int';
    }

    public function isFloat(): bool {
        return $this->isBuiltin && $this->name === 'float';
    }

    public function isString(): bool {
        return $this->isBuiltin && $this->name === 'string';
    }

    public function isArray(): bool {
        return $this->isBuiltin && $this->name === 'array';
    }

    public function toTypeCode(): string {
        assert($this->isBuiltin);
        switch ($this->name) {
            case "bool":
                return "_IS_BOOL";
            case "int":
                return "IS_LONG";
            case "float":
                return "IS_DOUBLE";
            case "string":
                return "IS_STRING";
            case "array":
                return "IS_ARRAY";
            case "object":
                return "IS_OBJECT";
            case "void":
                return "IS_VOID";
            case "callable":
                return "IS_CALLABLE";
            case "mixed":
                return "IS_MIXED";
            case "static":
                return "IS_STATIC";
            case "never":
                return "IS_NEVER";
            case "null":
                return "IS_NULL";
            case "false":
                return "IS_FALSE";
            case "true":
                return "IS_TRUE";
            default:
                throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeMask(): string {
        assert($this->isBuiltin);

        switch ($this->name) {
            case "null":
                return "MAY_BE_NULL";
            case "false":
                return "MAY_BE_FALSE";
            case "true":
                return "MAY_BE_TRUE";
            case "bool":
                return "MAY_BE_BOOL";
            case "int":
                return "MAY_BE_LONG";
            case "float":
                return "MAY_BE_DOUBLE";
            case "string":
                return "MAY_BE_STRING";
            case "array":
                return "MAY_BE_ARRAY";
            case "object":
                return "MAY_BE_OBJECT";
            case "callable":
                return "MAY_BE_CALLABLE";
            case "mixed":
                return "MAY_BE_ANY";
            case "void":
                return "MAY_BE_VOID";
            case "static":
                return "MAY_BE_STATIC";
            case "never":
                return "MAY_BE_NEVER";
            default:
                throw new Exception("Not implemented: $this->name");
        }
    }

    public function toOptimizerTypeMaskForArrayKey(): string {
        assert($this->isBuiltin);

        switch ($this->name) {
            case "int":
                return "MAY_BE_ARRAY_KEY_LONG";
            case "string":
                return "MAY_BE_ARRAY_KEY_STRING";
            default:
                throw new Exception("Type $this->name cannot be an array key");
        }
    }

    public function toOptimizerTypeMaskForArrayValue(): string {
        if (!$this->isBuiltin) {
            return "MAY_BE_ARRAY_OF_OBJECT";
        }

        switch ($this->name) {
            case "null":
                return "MAY_BE_ARRAY_OF_NULL";
            case "false":
                return "MAY_BE_ARRAY_OF_FALSE";
            case "true":
                return "MAY_BE_ARRAY_OF_TRUE";
            case "bool":
                return "MAY_BE_ARRAY_OF_FALSE|MAY_BE_ARRAY_OF_TRUE";
            case "int":
                return "MAY_BE_ARRAY_OF_LONG";
            case "float":
                return "MAY_BE_ARRAY_OF_DOUBLE";
            case "string":
                return "MAY_BE_ARRAY_OF_STRING";
            case "array":
                return "MAY_BE_ARRAY_OF_ARRAY";
            case "object":
                return "MAY_BE_ARRAY_OF_OBJECT";
            case "resource":
                return "MAY_BE_ARRAY_OF_RESOURCE";
            case "mixed":
                return "MAY_BE_ARRAY_OF_ANY";
            case "ref":
                return "MAY_BE_ARRAY_OF_REF";
            default:
                throw new Exception("Type $this->name cannot be an array value");
        }
    }

    public function toOptimizerTypeMask(): string {
        if (!$this->isBuiltin) {
            return "MAY_BE_OBJECT";
        }

        switch ($this->name) {
            case "resource":
                return "MAY_BE_RESOURCE";
            case "callable":
                return "MAY_BE_STRING|MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_LONG|MAY_BE_ARRAY_OF_STRING|MAY_BE_ARRAY_OF_OBJECT|MAY_BE_OBJECT";
            case "iterable":
                return "MAY_BE_ARRAY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY|MAY_BE_OBJECT";
            case "mixed":
                return "MAY_BE_ANY|MAY_BE_ARRAY_KEY_ANY|MAY_BE_ARRAY_OF_ANY";
        }

        return $this->toTypeMask();
    }

    public function toEscapedName(): string {
        // Escape backslashes, and also encode \u, \U, and \N to avoid compilation errors in generated macros
        return str_replace(
            ['\\', '\\u', '\\U', '\\N'],
            ['\\\\', '\\\\165', '\\\\125', '\\\\116'],
            $this->name
        );
    }

    public function toVarEscapedName(): string {
        return str_replace('\\', '_', $this->name);
    }

    public function equals(SimpleType $other): bool {
        return $this->name === $other->name && $this->isBuiltin === $other->isBuiltin;
    }
}

class Type {
    /** @var SimpleType[] */
    public array $types;
    public bool $isIntersection;

    public static function fromNode(Node $node): Type {
        if ($node instanceof Node\UnionType || $node instanceof Node\IntersectionType) {
            $nestedTypeObjects = array_map(['Type', 'fromNode'], $node->types);
            $types = [];
            foreach ($nestedTypeObjects as $typeObject) {
                array_push($types, ...$typeObject->types);
            }
            return new Type($types, ($node instanceof Node\IntersectionType));
        }

        if ($node instanceof Node\NullableType) {
            return new Type(
                [
                    ...Type::fromNode($node->type)->types,
                    SimpleType::null(),
                ],
                false
            );
        }

        if ($node instanceof Node\Identifier && $node->toLowerString() === "iterable") {
            return new Type(
                [
                    SimpleType::fromString("Traversable"),
                    ArrayType::createGenericArray(),
                ],
                false
            );
        }

        return new Type([SimpleType::fromNode($node)], false);
    }

    public static function fromString(string $typeString): self {
        $typeString .= "|";
        $simpleTypes = [];
        $simpleTypeOffset = 0;
        $inArray = false;
        $isIntersection = false;

        $typeStringLength = strlen($typeString);
        for ($i = 0; $i < $typeStringLength; $i++) {
            $char = $typeString[$i];

            if ($char === "<") {
                $inArray = true;
                continue;
            }

            if ($char === ">") {
                $inArray = false;
                continue;
            }

            if ($inArray) {
                continue;
            }

            if ($char === "|" || $char === "&") {
                $isIntersection = ($char === "&");
                $simpleTypeName = trim(substr($typeString, $simpleTypeOffset, $i - $simpleTypeOffset));

                $simpleTypes[] = SimpleType::fromString($simpleTypeName);

                $simpleTypeOffset = $i + 1;
            }
        }

        return new Type($simpleTypes, $isIntersection);
    }

    /**
     * @param SimpleType[] $types
     */
    private function __construct(array $types, bool $isIntersection) {
        $this->types = $types;
        $this->isIntersection = $isIntersection;
    }

    public function isScalar(): bool {
        foreach ($this->types as $type) {
            if (!$type->isScalar()) {
                return false;
            }
        }

        return true;
    }

    public function isNullable(): bool {
        foreach ($this->types as $type) {
            if ($type->isNull()) {
                return true;
            }
        }

        return false;
    }

    public function getWithoutNull(): Type {
        return new Type(
            array_values(
                array_filter(
                    $this->types,
                    function(SimpleType $type) {
                        return !$type->isNull();
                    }
                )
            ),
            false
        );
    }

    public function tryToSimpleType(): ?SimpleType {
        $withoutNull = $this->getWithoutNull();
        /* type has only null */
        if (count($withoutNull->types) === 0) {
            return $this->types[0];
        }
        if (count($withoutNull->types) === 1) {
            return $withoutNull->types[0];
        }
        return null;
    }

    public function toArginfoType(): ArginfoType {
        $classTypes = [];
        $builtinTypes = [];
        foreach ($this->types as $type) {
            if ($type->isBuiltin) {
                $builtinTypes[] = $type;
            } else {
                $classTypes[] = $type;
            }
        }
        return new ArginfoType($classTypes, $builtinTypes);
    }

    public function toOptimizerTypeMask(): string {
        $optimizerTypes = [];

        foreach ($this->types as $type) {
            // TODO Support for toOptimizerMask for intersection
            $optimizerTypes[] = $type->toOptimizerTypeMask();
        }

        return implode("|", $optimizerTypes);
    }

    public function toOptimizerTypeMaskForArrayKey(): string {
        $typeMasks = [];

        foreach ($this->types as $type) {
            $typeMasks[] = $type->toOptimizerTypeMaskForArrayKey();
        }

        return implode("|", $typeMasks);
    }

    public function toOptimizerTypeMaskForArrayValue(): string {
        $typeMasks = [];

        foreach ($this->types as $type) {
            $typeMasks[] = $type->toOptimizerTypeMaskForArrayValue();
        }

        return implode("|", $typeMasks);
    }

    public function getTypeForDoc(DOMDocument $doc): DOMElement {
        if (count($this->types) > 1) {
            $typeSort = $this->isIntersection ? "intersection" : "union";
            $typeElement = $doc->createElement('type');
            $typeElement->setAttribute("class", $typeSort);

            foreach ($this->types as $type) {
                $unionTypeElement = $doc->createElement('type', $type->name);
                $typeElement->appendChild($unionTypeElement);
            }
        } else {
            $type = $this->types[0];
            $name = $type->name;

            $typeElement = $doc->createElement('type', $name);
        }

        return $typeElement;
    }

    public static function equals(?Type $a, ?Type $b): bool {
        if ($a === null || $b === null) {
            return $a === $b;
        }

        if (count($a->types) !== count($b->types)) {
            return false;
        }

        for ($i = 0; $i < count($a->types); $i++) {
            if (!$a->types[$i]->equals($b->types[$i])) {
                return false;
            }
        }

        return true;
    }

    public function __toString() {
        if ($this->types === null) {
            return 'mixed';
        }

        $char = $this->isIntersection ? '&' : '|';
        return implode($char, array_map(
            function ($type) { return $type->name; },
            $this->types)
        );
    }
}

class ArginfoType {
    /** @var SimpleType[] $classTypes */
    public array $classTypes;
    /** @var SimpleType[] $builtinTypes */
    private array $builtinTypes;

    /**
     * @param SimpleType[] $classTypes
     * @param SimpleType[] $builtinTypes
     */
    public function __construct(array $classTypes, array $builtinTypes) {
        $this->classTypes = $classTypes;
        $this->builtinTypes = $builtinTypes;
    }

    public function hasClassType(): bool {
        return !empty($this->classTypes);
    }

    public function toClassTypeString(): string {
        return implode('|', array_map(function(SimpleType $type) {
            return $type->toEscapedName();
        }, $this->classTypes));
    }

    public function toTypeMask(): string {
        if (empty($this->builtinTypes)) {
            return '0';
        }
        return implode('|', array_map(function(SimpleType $type) {
            return $type->toTypeMask();
        }, $this->builtinTypes));
    }
}

class ArgInfo {
    const SEND_BY_VAL = 0;
    const SEND_BY_REF = 1;
    const SEND_PREFER_REF = 2;

    public string $name;
    public int $sendBy;
    public bool $isVariadic;
    public ?Type $type;
    public ?Type $phpDocType;
    public ?string $defaultValue;
    /** @var AttributeInfo[] */
    public array $attributes;

    /**
     * @param AttributeInfo[] $attributes
     */
    public function __construct(
        string $name,
        int $sendBy,
        bool $isVariadic,
        ?Type $type,
        ?Type $phpDocType,
        ?string $defaultValue,
        array $attributes
    ) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->setTypes($type, $phpDocType);
        $this->defaultValue = $defaultValue;
        $this->attributes = $attributes;
    }

    public function equals(ArgInfo $other): bool {
        return $this->name === $other->name
            && $this->sendBy === $other->sendBy
            && $this->isVariadic === $other->isVariadic
            && Type::equals($this->type, $other->type)
            && $this->defaultValue === $other->defaultValue;
    }

    public function getSendByString(): string {
        switch ($this->sendBy) {
        case self::SEND_BY_VAL:
            return "0";
        case self::SEND_BY_REF:
            return "1";
        case self::SEND_PREFER_REF:
            return "ZEND_SEND_PREFER_REF";
        }
        throw new Exception("Invalid sendBy value");
    }

    public function getMethodSynopsisType(): Type {
        if ($this->type) {
            return $this->type;
        }

        if ($this->phpDocType) {
            return $this->phpDocType;
        }

        throw new Exception("A parameter must have a type");
    }

    public function hasProperDefaultValue(): bool {
        return $this->defaultValue !== null && $this->defaultValue !== "UNKNOWN";
    }

    public function getDefaultValueAsArginfoString(): string {
        if ($this->hasProperDefaultValue()) {
            return '"' . addslashes($this->defaultValue) . '"';
        }

        return "NULL";
    }

    public function getDefaultValueAsMethodSynopsisString(): ?string {
        if ($this->defaultValue === null) {
            return null;
        }

        switch ($this->defaultValue) {
            case 'UNKNOWN':
                return null;
            case 'false':
            case 'true':
            case 'null':
                return "&{$this->defaultValue};";
        }

        return $this->defaultValue;
    }

    private function setTypes(?Type $type, ?Type $phpDocType): void
    {
        $this->type = $type;
        $this->phpDocType = $phpDocType;
    }
}

interface VariableLikeName {
    public function __toString(): string;
    public function getDeclarationName(): string;
}

interface ConstOrClassConstName extends VariableLikeName {
    public function equals(ConstOrClassConstName $const): bool;
    public function isClassConst(): bool;
    public function isUnknown(): bool;
}

abstract class AbstractConstName implements ConstOrClassConstName
{
    public function equals(ConstOrClassConstName $const): bool
    {
        return $this->__toString() === $const->__toString();
    }

    public function isUnknown(): bool
    {
        return strtolower($this->__toString()) === "unknown";
    }
}

class ConstName extends AbstractConstName {
    public string $const;

    public function __construct(?Name $namespace, string $const)
    {
        if ($namespace && ($namespace = $namespace->slice(0, -1))) {
            $const = $namespace->toString() . '\\' . $const;
        }
        $this->const = $const;
    }

    public function isClassConst(): bool
    {
        return false;
    }

    public function isUnknown(): bool
    {
        $name = $this->__toString();
        if (($pos = strrpos($name, '\\')) !== false) {
            $name = substr($name, $pos + 1);
        }
        return strtolower($name) === "unknown";
    }

    public function __toString(): string
    {
        return $this->const;
    }

    public function getDeclarationName(): string
    {
        return $this->name->toString();
    }
}

class ClassConstName extends AbstractConstName {
    public Name $class;
    public string $const;

    public function __construct(Name $class, string $const)
    {
        $this->class = $class;
        $this->const = $const;
    }

    public function isClassConst(): bool
    {
        return true;
    }

    public function __toString(): string
    {
        return $this->class->toString() . "::" . $this->const;
    }

    public function getDeclarationName(): string
    {
        return $this->const;
    }
}

class PropertyName implements VariableLikeName {
    public Name $class;
    public string $property;

    public function __construct(Name $class, string $property)
    {
        $this->class = $class;
        $this->property = $property;
    }

    public function __toString()
    {
        return $this->class->toString() . "::$" . $this->property;
    }

    public function getDeclarationName(): string
    {
         return $this->property;
    }
}

interface FunctionOrMethodName {
    public function getDeclaration(): string;
    public function getArgInfoName(): string;
    public function getMethodSynopsisFilename(): string;
    public function getNameForAttributes(): string;
    public function __toString(): string;
    public function isMethod(): bool;
    public function isConstructor(): bool;
    public function isDestructor(): bool;
}

class FunctionName implements FunctionOrMethodName {
    private Name $name;

    public function __construct(Name $name) {
        $this->name = $name;
    }

    public function getNamespace(): ?string {
        if ($this->name->isQualified()) {
            return $this->name->slice(0, -1)->toString();
        }
        return null;
    }

    public function getNonNamespacedName(): string {
        if ($this->name->isQualified()) {
            throw new Exception("Namespaced name not supported here");
        }
        return $this->name->toString();
    }

    public function getDeclarationName(): string {
        return implode('_', $this->name->getParts());
    }

    public function getFunctionName(): string {
        return $this->name->getLast();
    }

    public function getDeclaration(): string {
        return "ZEND_FUNCTION({$this->getDeclarationName()});\n";
    }

    public function getArgInfoName(): string {
        $underscoreName = implode('_', $this->name->getParts());
        return "arginfo_$underscoreName";
    }

    public function getMethodSynopsisFilename(): string {
        return 'functions/' . implode('/', str_replace('_', '-', $this->name->getParts()));
    }

    public function getNameForAttributes(): string {
        return strtolower($this->name->toString());
    }

    public function __toString(): string {
        return $this->name->toString();
    }

    public function isMethod(): bool {
        return false;
    }

    public function isConstructor(): bool {
        return false;
    }

    public function isDestructor(): bool {
        return false;
    }
}

class MethodName implements FunctionOrMethodName {
    public Name $className;
    public string $methodName;

    public function __construct(Name $className, string $methodName) {
        $this->className = $className;
        $this->methodName = $methodName;
    }

    public function getDeclarationClassName(): string {
        return implode('_', $this->className->getParts());
    }

    public function getDeclaration(): string {
        return "ZEND_METHOD({$this->getDeclarationClassName()}, $this->methodName);\n";
    }

    public function getArgInfoName(): string {
        return "arginfo_class_{$this->getDeclarationClassName()}_{$this->methodName}";
    }

    public function getMethodSynopsisFilename(): string
    {
        $parts = [...$this->className->getParts(), ltrim($this->methodName, '_')];
        /* File paths are in lowercase */
        return strtolower(implode('/', $parts));
    }

    public function getNameForAttributes(): string {
        return strtolower($this->methodName);
    }

    public function __toString(): string {
        return "$this->className::$this->methodName";
    }

    public function isMethod(): bool {
        return true;
    }

    public function isConstructor(): bool {
        return $this->methodName === "__construct";
    }

    public function isDestructor(): bool {
        return $this->methodName === "__destruct";
    }
}

class ReturnInfo {
    const REFCOUNT_0 = "0";
    const REFCOUNT_1 = "1";
    const REFCOUNT_N = "N";

    const REFCOUNTS = [
        self::REFCOUNT_0,
        self::REFCOUNT_1,
        self::REFCOUNT_N,
    ];

    public bool $byRef;
    public ?Type $type;
    public ?Type $phpDocType;
    public bool $tentativeReturnType;
    public string $refcount;

    public function __construct(bool $byRef, ?Type $type, ?Type $phpDocType, bool $tentativeReturnType, ?string $refcount) {
        $this->byRef = $byRef;
        $this->setTypes($type, $phpDocType, $tentativeReturnType);
        $this->setRefcount($refcount);
    }

    public function equalsApartFromPhpDocAndRefcount(ReturnInfo $other): bool {
        return $this->byRef === $other->byRef
            && Type::equals($this->type, $other->type)
            && $this->tentativeReturnType === $other->tentativeReturnType;
    }

    public function getMethodSynopsisType(): ?Type {
        return $this->type ?? $this->phpDocType;
    }

    private function setTypes(?Type $type, ?Type $phpDocType, bool $tentativeReturnType): void
    {
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->tentativeReturnType = $tentativeReturnType;
    }

    private function setRefcount(?string $refcount): void
    {
        $type = $this->phpDocType ?? $this->type;
        $isScalarType = $type !== null && $type->isScalar();

        if ($refcount === null) {
            $this->refcount = $isScalarType ? self::REFCOUNT_0 : self::REFCOUNT_N;
            return;
        }

        if (!in_array($refcount, ReturnInfo::REFCOUNTS, true)) {
            throw new Exception("@refcount must have one of the following values: \"0\", \"1\", \"N\", $refcount given");
        }

        if ($isScalarType && $refcount !== self::REFCOUNT_0) {
            throw new Exception('A scalar return type of "' . $type->__toString() . '" must have a refcount of "' . self::REFCOUNT_0 . '"');
        }

        if (!$isScalarType && $refcount === self::REFCOUNT_0) {
            throw new Exception('A non-scalar return type of "' . $type->__toString() . '" cannot have a refcount of "' . self::REFCOUNT_0 . '"');
        }

        $this->refcount = $refcount;
    }
}

class FuncInfo {
    public FunctionOrMethodName $name;
    public int $classFlags;
    public int $flags;
    public ?string $aliasType;
    public ?FunctionOrMethodName $alias;
    public bool $isDeprecated;
    public bool $supportsCompileTimeEval;
    public bool $verify;
    /** @var ArgInfo[] */
    public array $args;
    public ReturnInfo $return;
    public int $numRequiredArgs;
    public ?string $cond;
    public bool $isUndocumentable;
    /** @var AttributeInfo[] */
    public array $attributes;

    /**
     * @param AttributeInfo[] $attributes
     * @param ArgInfo[] $args
     */
    public function __construct(
        FunctionOrMethodName $name,
        int $classFlags,
        int $flags,
        ?string $aliasType,
        ?FunctionOrMethodName $alias,
        bool $isDeprecated,
        bool $supportsCompileTimeEval,
        bool $verify,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond,
        bool $isUndocumentable,
        array $attributes
    ) {
        $this->name = $name;
        $this->classFlags = $classFlags;
        $this->flags = $flags;
        $this->aliasType = $aliasType;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
        $this->supportsCompileTimeEval = $supportsCompileTimeEval;
        $this->verify = $verify;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
        $this->isUndocumentable = $isUndocumentable;
        $this->attributes = $attributes;
    }

    public function isMethod(): bool
    {
        return $this->name->isMethod();
    }

    public function isFinalMethod(): bool
    {
        return ($this->flags & Class_::MODIFIER_FINAL) || ($this->classFlags & Class_::MODIFIER_FINAL);
    }

    public function isInstanceMethod(): bool
    {
        return !($this->flags & Class_::MODIFIER_STATIC) && $this->isMethod() && !$this->name->isConstructor();
    }

    /** @return string[] */
    public function getModifierNames(): array
    {
        if (!$this->isMethod()) {
            return [];
        }

        $result = [];

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $result[] = "final";
        } elseif ($this->flags & Class_::MODIFIER_ABSTRACT && $this->classFlags & ~Class_::MODIFIER_ABSTRACT) {
            $result[] = "abstract";
        }

        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $result[] = "protected";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $result[] = "private";
        } else {
            $result[] = "public";
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $result[] = "static";
        }

        return $result;
    }

    public function hasParamWithUnknownDefaultValue(): bool
    {
        foreach ($this->args as $arg) {
            if ($arg->defaultValue && !$arg->hasProperDefaultValue()) {
                return true;
            }
        }

        return false;
    }

    public function equalsApartFromNameAndRefcount(FuncInfo $other): bool {
        if (count($this->args) !== count($other->args)) {
            return false;
        }

        for ($i = 0; $i < count($this->args); $i++) {
            if (!$this->args[$i]->equals($other->args[$i])) {
                return false;
            }
        }

        return $this->return->equalsApartFromPhpDocAndRefcount($other->return)
            && $this->numRequiredArgs === $other->numRequiredArgs
            && $this->cond === $other->cond;
    }

    public function getArgInfoName(): string {
        return $this->name->getArgInfoName();
    }

    public function getDeclarationKey(): string
    {
        $name = $this->alias ?? $this->name;

        return "$name|$this->cond";
    }

    public function getDeclaration(): ?string
    {
        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            return null;
        }

        $name = $this->alias ?? $this->name;

        return $name->getDeclaration();
    }

    public function getFunctionEntry(): string {
        if ($this->name instanceof MethodName) {
            if ($this->alias) {
                if ($this->alias instanceof MethodName) {
                    return sprintf(
                        "\tZEND_MALIAS(%s, %s, %s, %s, %s)\n",
                        $this->alias->getDeclarationClassName(), $this->name->methodName,
                        $this->alias->methodName, $this->getArgInfoName(), $this->getFlagsAsArginfoString()
                    );
                } else if ($this->alias instanceof FunctionName) {
                    return sprintf(
                        "\tZEND_ME_MAPPING(%s, %s, %s, %s)\n",
                        $this->name->methodName, $this->alias->getNonNamespacedName(),
                        $this->getArgInfoName(), $this->getFlagsAsArginfoString()
                    );
                } else {
                    throw new Error("Cannot happen");
                }
            } else {
                $declarationClassName = $this->name->getDeclarationClassName();
                if ($this->flags & Class_::MODIFIER_ABSTRACT) {
                    return sprintf(
                        "\tZEND_ABSTRACT_ME_WITH_FLAGS(%s, %s, %s, %s)\n",
                        $declarationClassName, $this->name->methodName, $this->getArgInfoName(),
                        $this->getFlagsAsArginfoString()
                    );
                }

                return sprintf(
                    "\tZEND_ME(%s, %s, %s, %s)\n",
                    $declarationClassName, $this->name->methodName, $this->getArgInfoName(),
                    $this->getFlagsAsArginfoString()
                );
            }
        } else if ($this->name instanceof FunctionName) {
            $namespace = $this->name->getNamespace();
            $functionName = $this->name->getFunctionName();
            $declarationName = $this->alias ? $this->alias->getNonNamespacedName() : $this->name->getDeclarationName();

            if ($namespace) {
                // Namespaced functions are always declared as aliases to avoid name conflicts when two functions with
                // the same name exist in separate namespaces
                $macro = $this->isDeprecated ? 'ZEND_NS_DEP_FALIAS' : 'ZEND_NS_FALIAS';

                // Render A\B as "A\\B" in C strings for namespaces
                return sprintf(
                    "\t%s(\"%s\", %s, %s, %s)\n",
                    $macro, addslashes($namespace), $this->name->getFunctionName(), $declarationName, $this->getArgInfoName()
                );
            }

            if ($this->alias) {
                $macro = $this->isDeprecated ? 'ZEND_DEP_FALIAS' : 'ZEND_FALIAS';

                return sprintf(
                    "\t%s(%s, %s, %s)\n",
                    $macro, $functionName, $declarationName, $this->getArgInfoName()
                );
            }

            switch (true) {
                case $this->isDeprecated:
                    $macro = 'ZEND_DEP_FE';
                    break;
                case $this->supportsCompileTimeEval:
                    $macro = 'ZEND_SUPPORTS_COMPILE_TIME_EVAL_FE';
                    break;
                default:
                    $macro = 'ZEND_FE';
            }

            return sprintf("\t%s(%s, %s)\n", $macro, $functionName, $this->getArgInfoName());
        } else {
            throw new Error("Cannot happen");
        }
    }

    public function getOptimizerInfo(): ?string {
        if ($this->isMethod()) {
            return null;
        }

        if ($this->alias !== null) {
            return null;
        }

        if ($this->return->refcount !== ReturnInfo::REFCOUNT_1 && $this->return->phpDocType === null) {
            return null;
        }

        $type = $this->return->phpDocType ?? $this->return->type;
        if ($type === null) {
            return null;
        }

        return "\tF" . $this->return->refcount . '("' . $this->name->__toString() . '", ' . $type->toOptimizerTypeMask() . "),\n";
    }

    public function discardInfoForOldPhpVersions(): void {
        $this->attributes = [];
        $this->return->type = null;
        foreach ($this->args as $arg) {
            $arg->type = null;
            $arg->defaultValue = null;
            $arg->attributes = [];
        }
    }

    private function getFlagsAsArginfoString(): string
    {
        $flags = "ZEND_ACC_PUBLIC";
        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $flags = "ZEND_ACC_PROTECTED";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $flags = "ZEND_ACC_PRIVATE";
        }

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $flags .= "|ZEND_ACC_STATIC";
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $flags .= "|ZEND_ACC_FINAL";
        }

        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            $flags .= "|ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $flags .= "|ZEND_ACC_DEPRECATED";
        }

        return $flags;
    }

    private function generateRefSect1(DOMDocument $doc, string $role): DOMElement {
        $refSec = $doc->createElement('refsect1');
        $refSec->setAttribute('role', $role);
        $refSec->append(
            "\n  ",
            $doc->createEntityReference('reftitle.' . $role),
            "\n  "
        );
        return $refSec;
    }

    /**
     * @param array<string, FuncInfo> $funcMap
     * @param array<string, FuncInfo> $aliasMap
     * @throws Exception
     */
    public function getMethodSynopsisDocument(array $funcMap, array $aliasMap): ?string {
        $REFSEC1_SEPERATOR = "\n\n ";

        $doc = new DOMDocument("1.0", "utf-8");
        $doc->formatOutput = true;

        $refentry = $doc->createElement('refentry');
        $doc->appendChild($refentry);

        if ($this->isMethod()) {
            assert($this->name instanceof MethodName);
            /* Namespaces are seperated by '-', '_' must be converted to '-' too.
             * Trim away the __ for magic methods */
            $id = strtolower(
                str_replace('\\', '-', $this->name->className->__toString())
                . '.'
                . str_replace('_', '-', ltrim($this->name->methodName, '_'))
            );
        } else {
            $id = 'function.' . strtolower(str_replace('_', '-', $this->name->__toString()));
        }
        $refentry->setAttribute("xml:id", $id);
        /* We create an attribute for xmlns, as libxml otherwise force it to be the first one */
        //$refentry->setAttribute("xmlns", "http://docbook.org/ns/docbook");
        $namespace = $doc->createAttribute('xmlns');
        $namespace->value = "http://docbook.org/ns/docbook";
        $refentry->setAttributeNode($namespace);
        $refentry->setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
        $refentry->appendChild(new DOMText("\n "));

        /* Creation of <refnamediv> */
        $refnamediv = $doc->createElement('refnamediv');
        $refnamediv->appendChild(new DOMText("\n  "));
        $refname = $doc->createElement('refname', $this->name->__toString());
        $refnamediv->appendChild($refname);
        $refnamediv->appendChild(new DOMText("\n  "));
        $refpurpose = $doc->createElement('refpurpose', 'Description');
        $refnamediv->appendChild($refpurpose);

        $refnamediv->appendChild(new DOMText("\n "));
        $refentry->append($refnamediv, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="description"> */
        $descriptionRefSec = $this->generateRefSect1($doc, 'description');

        $methodSynopsis = $this->getMethodSynopsisElement($funcMap, $aliasMap, $doc);
        if (!$methodSynopsis) {
            return null;
        }
        $descriptionRefSec->appendChild($methodSynopsis);
        $descriptionRefSec->appendChild(new DOMText("\n  "));
        $undocumentedEntity = $doc->createEntityReference('warn.undocumented.func');
        $descriptionRefSec->appendChild($undocumentedEntity);
        $descriptionRefSec->appendChild(new DOMText("\n  "));
        $returnDescriptionPara = $doc->createElement('para');
        $returnDescriptionPara->appendChild(new DOMText("\n   Description\n  "));
        $descriptionRefSec->appendChild($returnDescriptionPara);

        $descriptionRefSec->appendChild(new DOMText("\n "));
        $refentry->append($descriptionRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="parameters"> */
        $parametersRefSec = $this->getParameterSection($doc);
        $refentry->append($parametersRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="returnvalues"> */
        if (!$this->name->isConstructor() && !$this->name->isDestructor()) {
            $returnRefSec = $this->getReturnValueSection($doc);
            $refentry->append($returnRefSec, $REFSEC1_SEPERATOR);
        }

        /* Creation of <refsect1 role="errors"> */
        $errorsRefSec = $this->generateRefSect1($doc, 'errors');
        $errorsDescriptionPara = $doc->createElement('para');
        $errorsDescriptionPara->appendChild(new DOMText("\n   When does this function issue E_* level errors, and/or throw exceptions.\n  "));
        $errorsRefSec->appendChild($errorsDescriptionPara);
        $errorsRefSec->appendChild(new DOMText("\n "));

        $refentry->append($errorsRefSec, $REFSEC1_SEPERATOR);

        /* Creation of <refsect1 role="changelog"> */
        $changelogRefSec = $this->getChangelogSection($doc);
        $refentry->append($changelogRefSec, $REFSEC1_SEPERATOR);

        $exampleRefSec = $this->getExampleSection($doc);
        $refentry->append($exampleRefSec, $REFSEC1_SEPERATOR);

        // TODO Notes section?

        /* Creation of <refsect1 role="seealso"> */
        $seeAlsoRefSec = $this->generateRefSect1($doc, 'seealso');

        /* TODO Actually generate a markup for class names, functions and links?
  <simplelist>
   <member><methodname>ClassName::otherMethodName</methodname></member>
   <member><function>some_function</function></member>
   <member>The <link linkend="something">something appendix</link></member>
  </simplelist>
         */
        $seeAlsoMember = $doc->createElement('member');
        $seeAlsoMember->appendChild(new DOMText("Method name, function, or link to reference"));

        $seeAlsoList = $doc->createElement('simplelist');
        $seeAlsoList->appendChild(new DOMText("\n   "));
        $seeAlsoList->appendChild($seeAlsoMember);
        $seeAlsoList->appendChild(new DOMText("\n  "));

        $seeAlsoRefSec->appendChild($seeAlsoList);
        $seeAlsoRefSec->appendChild(new DOMText("\n "));

        $refentry->appendChild($seeAlsoRefSec);

        $refentry->appendChild(new DOMText("\n\n"));

        $doc->appendChild(new DOMComment(
            <<<ENDCOMMENT
Keep this comment at the end of the file
Local variables:
mode: sgml
sgml-omittag:t
sgml-shorttag:t
sgml-minimize-attributes:nil
sgml-always-quote-attributes:t
sgml-indent-step:1
sgml-indent-data:t
indent-tabs-mode:nil
sgml-parent-document:nil
sgml-default-dtd-file:"~/.phpdoc/manual.ced"
sgml-exposed-tags:nil
sgml-local-catalogs:nil
sgml-local-ecat-files:nil
End:
vim600: syn=xml fen fdm=syntax fdl=2 si
vim: et tw=78 syn=sgml
vi: ts=1 sw=1

ENDCOMMENT
        ));
        return $doc->saveXML();
    }

    private function getParameterSection(DOMDocument $doc): DOMElement {
        $parametersRefSec = $this->generateRefSect1($doc, 'parameters');
        if (empty($this->args)) {
            $noParamEntity = $doc->createEntityReference('no.function.parameters');
            $parametersRefSec->appendChild($noParamEntity);
            return $parametersRefSec;
        } else {
            $parametersPara = $doc->createElement('para');
            $parametersRefSec->appendChild($parametersPara);

            $parametersPara->appendChild(new DOMText("\n   "));
            $parametersList = $doc->createElement('variablelist');
            $parametersPara->appendChild($parametersList);

            /*
            <varlistentry>
             <term><parameter>name</parameter></term>
             <listitem>
              <para>
               Description.
              </para>
             </listitem>
            </varlistentry>
            */
            foreach ($this->args as $arg) {
                $parameter = $doc->createElement('parameter', $arg->name);
                $parameterTerm = $doc->createElement('term');
                $parameterTerm->appendChild($parameter);

                $parameterEntry = $doc->createElement('varlistentry');
                $parameterEntry->appendChild(new DOMText("\n     "));
                $parameterEntry->appendChild($parameterTerm);
                $parameterEntry->appendChild(new DOMText("\n     "));

                $listItemPara = $doc->createElement('para');
                $listItemPara->appendChild(new DOMText("\n       "));
                $listItemPara->appendChild(new DOMText("Description."));
                $listItemPara->appendChild(new DOMText("\n      "));

                $parameterEntryListItem = $doc->createElement('listitem');
                $parameterEntryListItem->appendChild(new DOMText("\n      "));
                $parameterEntryListItem->appendChild($listItemPara);
                $parameterEntryListItem->appendChild(new DOMText("\n     "));

                $parameterEntry->appendChild($parameterEntryListItem);
                $parameterEntry->appendChild(new DOMText("\n    "));

                $parametersList->appendChild(new DOMText("\n    "));
                $parametersList->appendChild($parameterEntry);
            }
            $parametersList->appendChild(new DOMText("\n   "));
        }
        $parametersPara->appendChild(new DOMText("\n  "));
        $parametersRefSec->appendChild(new DOMText("\n "));
        return $parametersRefSec;
    }

    private function getReturnValueSection(DOMDocument $doc): DOMElement {
        $returnRefSec = $this->generateRefSect1($doc, 'returnvalues');

        $returnDescriptionPara = $doc->createElement('para');
        $returnDescriptionPara->appendChild(new DOMText("\n   "));

        $returnType = $this->return->type;
        if ($returnType === null) {
            $returnDescriptionPara->appendChild(new DOMText("Description"));
        } else if (count($returnType->types) === 1) {
            $type = $returnType->types[0];
            $name = $type->name;
            $descriptionNode = match ($name) {
                'void' => $doc->createEntityReference('return.void'),
                'true' => $doc->createEntityReference('return.true.always'),
                'bool' => $doc->createEntityReference('return.success'),
                default => new DOMText("Description"),
            };
            $returnDescriptionPara->appendChild($descriptionNode);
        } else {
            $returnDescriptionPara->appendChild(new DOMText("Description"));
        }
        $returnDescriptionPara->appendChild(new DOMText("\n  "));
        $returnRefSec->appendChild($returnDescriptionPara);
        $returnRefSec->appendChild(new DOMText("\n "));
        return $returnRefSec;
    }

    /**
     * @param array<DOMNode> $headers [count($headers) === $columns]
     * @param array<array<DOMNode>> $rows [count($rows[$i]) === $columns]
     */
    private function generateDocbookInformalTable(
        DOMDocument $doc,
        int $indent,
        int $columns,
        array $headers,
        array $rows
    ): DOMElement {
        $strIndent = str_repeat(' ', $indent);

        $headerRow = $doc->createElement('row');
        foreach ($headers as $header) {
            $headerEntry = $doc->createElement('entry');
            $headerEntry->appendChild($header);

            $headerRow->appendChild(new DOMText("\n$strIndent    "));
            $headerRow->appendChild($headerEntry);
        }
        $headerRow->appendChild(new DOMText("\n$strIndent   "));

        $thead = $doc->createElement('thead');
        $thead->appendChild(new DOMText("\n$strIndent   "));
        $thead->appendChild($headerRow);
        $thead->appendChild(new DOMText("\n$strIndent  "));

        $tbody = $doc->createElement('tbody');
        foreach ($rows as $row) {
            $bodyRow = $doc->createElement('row');
            foreach ($row as $cell) {
                $entry = $doc->createElement('entry');
                $entry->appendChild($cell);

                $bodyRow->appendChild(new DOMText("\n$strIndent    "));
                $bodyRow->appendChild($entry);
            }
            $bodyRow->appendChild(new DOMText("\n$strIndent   "));

            $tbody->appendChild(new DOMText("\n$strIndent   "));
            $tbody->appendChild($bodyRow);
            $tbody->appendChild(new DOMText("\n$strIndent  "));
        }

        $tgroup = $doc->createElement('tgroup');
        $tgroup->setAttribute('cols', (string) $columns);
        $tgroup->appendChild(new DOMText("\n$strIndent  "));
        $tgroup->appendChild($thead);
        $tgroup->appendChild(new DOMText("\n$strIndent  "));
        $tgroup->appendChild($tbody);
        $tgroup->appendChild(new DOMText("\n$strIndent "));

        $table = $doc->createElement('informaltable');
        $table->appendChild(new DOMText("\n$strIndent "));
        $table->appendChild($tgroup);
        $table->appendChild(new DOMText("\n$strIndent"));

        return $table;
    }

    private function getChangelogSection(DOMDocument $doc): DOMElement {
        $refSec = $this->generateRefSect1($doc, 'changelog');
        $headers = [
            $doc->createEntityReference('Version'),
            $doc->createEntityReference('Description'),
        ];
        $rows = [[
            new DOMText('8.X.0'),
            new DOMText("\n       Description\n      "),
        ]];
        $table = $this->generateDocbookInformalTable($doc, indent: 2, columns: 2, headers: $headers, rows: $rows);
        $refSec->appendChild($table);

        $refSec->appendChild(new DOMText("\n "));
        return $refSec;
    }

    private function getExampleSection(DOMDocument $doc): DOMElement {
        $refSec = $this->generateRefSect1($doc, 'examples');

        $example = $doc->createElement('example');
        $example->setAttribute('xml:id', 'func-or-method-name.example.basic');

        $title = $doc->createElement('title');
        $fn = $doc->createElement($this->isMethod() ? 'methodname' : 'function');
        $fn->append($this->name->__toString());
        $title->append($fn, ' example');

        $example->append("\n   ", $title);

        $para = $doc->createElement('para');
        $para->append("\n    ", "Description.", "\n   ");
        $example->append("\n   ", $para);

        $prog = $doc->createElement('programlisting');
        $prog->setAttribute('role', 'php');
        $code = new DOMCdataSection(
            <<<CODE_EXAMPLE

<?php
echo "Code example";
?>

CODE_EXAMPLE
        );
        $prog->append("\n");
        $prog->appendChild($code);
        $prog->append("\n   ");

        $example->append("\n   ", $prog);
        $example->append("\n   ", $doc->createEntityReference('example.outputs'));

        $output = new DOMCdataSection(
            <<<OUPUT_EXAMPLE

Code example

OUPUT_EXAMPLE
        );
        $screen = $doc->createElement('screen');
        $screen->append("\n");
        $screen->appendChild($output);
        $screen->append("\n   ");

        $example->append("\n   ", $screen);
        $example->append("\n  ");

        $refSec->append($example);

        $refSec->appendChild(new DOMText("\n "));
        return $refSec;
    }

    /**
     * @param array<string, FuncInfo> $funcMap
     * @param array<string, FuncInfo> $aliasMap
     * @throws Exception
     */
    public function getMethodSynopsisElement(array $funcMap, array $aliasMap, DOMDocument $doc): ?DOMElement {
        if ($this->hasParamWithUnknownDefaultValue()) {
            return null;
        }

        if ($this->name->isConstructor()) {
            $synopsisType = "constructorsynopsis";
        } elseif ($this->name->isDestructor()) {
            $synopsisType = "destructorsynopsis";
        } else {
            $synopsisType = "methodsynopsis";
        }

        $methodSynopsis = $doc->createElement($synopsisType);

        if ($this->isMethod()) {
            assert($this->name instanceof MethodName);
            $role = $doc->createAttribute("role");
            $role->value = addslashes($this->name->className->__toString());
            $methodSynopsis->appendChild($role);
        }

        $methodSynopsis->appendChild(new DOMText("\n   "));

        foreach ($this->getModifierNames() as $modifierString) {
            $modifierElement = $doc->createElement('modifier', $modifierString);
            $methodSynopsis->appendChild($modifierElement);
            $methodSynopsis->appendChild(new DOMText(" "));
        }

        $returnType = $this->return->getMethodSynopsisType();
        if ($returnType) {
            $methodSynopsis->appendChild($returnType->getTypeForDoc($doc));
        }

        $methodname = $doc->createElement('methodname', $this->name->__toString());
        $methodSynopsis->appendChild($methodname);

        if (empty($this->args)) {
            $methodSynopsis->appendChild(new DOMText("\n   "));
            $void = $doc->createElement('void');
            $methodSynopsis->appendChild($void);
        } else {
            foreach ($this->args as $arg) {
                $methodSynopsis->appendChild(new DOMText("\n   "));
                $methodparam = $doc->createElement('methodparam');
                if ($arg->defaultValue !== null) {
                    $methodparam->setAttribute("choice", "opt");
                }
                if ($arg->isVariadic) {
                    $methodparam->setAttribute("rep", "repeat");
                }

                $methodSynopsis->appendChild($methodparam);
                $methodparam->appendChild($arg->getMethodSynopsisType()->getTypeForDoc($doc));

                $parameter = $doc->createElement('parameter', $arg->name);
                if ($arg->sendBy !== ArgInfo::SEND_BY_VAL) {
                    $parameter->setAttribute("role", "reference");
                }

                $methodparam->appendChild($parameter);
                $defaultValue = $arg->getDefaultValueAsMethodSynopsisString();
                if ($defaultValue !== null) {
                    $initializer = $doc->createElement('initializer');
                    if (preg_match('/^[a-zA-Z_][a-zA-Z_0-9]*$/', $defaultValue)) {
                        $constant = $doc->createElement('constant', $defaultValue);
                        $initializer->appendChild($constant);
                    } else {
                        $initializer->nodeValue = $defaultValue;
                    }
                    $methodparam->appendChild($initializer);
                }
            }
        }
        $methodSynopsis->appendChild(new DOMText("\n  "));

        return $methodSynopsis;
    }

    public function __clone()
    {
        foreach ($this->args as $key => $argInfo) {
            $this->args[$key] = clone $argInfo;
        }
        $this->return = clone $this->return;
    }
}

class EvaluatedValue
{
    /** @var mixed */
    public $value;
    public SimpleType $type;
    public Expr $expr;
    public bool $isUnknownConstValue;
    /** @var ConstInfo[] */
    public array $originatingConsts;

    /**
     * @param array<string, ConstInfo> $allConstInfos
     */
    public static function createFromExpression(Expr $expr, ?SimpleType $constType, ?string $cConstName, array $allConstInfos): EvaluatedValue
    {
        // This visitor replaces the PHP constants by C constants. It allows direct expansion of the compiled constants, e.g. later in the pretty printer.
        $visitor = new class($allConstInfos) extends PhpParser\NodeVisitorAbstract
        {
            /** @var iterable<ConstInfo> */
            public array $visitedConstants = [];
            /** @var array<string, ConstInfo> */
            public array $allConstInfos;

            /** @param array<string, ConstInfo> $allConstInfos */
            public function __construct(array $allConstInfos)
            {
                $this->allConstInfos = $allConstInfos;
            }

            /** @return Node|null */
            public function enterNode(Node $expr)
            {
                if (!$expr instanceof Expr\ConstFetch && !$expr instanceof Expr\ClassConstFetch) {
                    return null;
                }

                if ($expr instanceof Expr\ClassConstFetch) {
                    $originatingConstName = new ClassConstName($expr->class, $expr->name->toString());
                } else {
                    $originatingConstName = new ConstName($expr->name->getAttribute('namespacedName'), $expr->name->toString());
                }

                if ($originatingConstName->isUnknown()) {
                    return null;
                }

                $const = $this->allConstInfos[$originatingConstName->__toString()] ?? null;
                if ($const !== null) {
                    $this->visitedConstants[] = $const;
                    return $const->getValue($this->allConstInfos)->expr;
                }
            }
        };

        $nodeTraverser = new PhpParser\NodeTraverser;
        $nodeTraverser->addVisitor($visitor);
        $expr = $nodeTraverser->traverse([$expr])[0];

        $isUnknownConstValue = false;

        $evaluator = new ConstExprEvaluator(
            static function (Expr $expr) use ($allConstInfos, &$isUnknownConstValue) {
                // $expr is a ConstFetch with a name of a C macro here
                if (!$expr instanceof Expr\ConstFetch) {
                    throw new Exception($this->getVariableTypeName() . " " . $this->name->__toString() . " has an unsupported value");
                }

                $constName = $expr->name->__toString();
                if (strtolower($constName) === "unknown") {
                    $isUnknownConstValue = true;
                    return null;
                }

                foreach ($allConstInfos as $const) {
                    if ($constName != $const->cValue) {
                        continue;
                    }

                    $constType = ($const->phpDocType ?? $const->type)->tryToSimpleType();
                    if ($constType) {
                        if ($constType->isBool()) {
                            return true;
                        } elseif ($constType->isInt()) {
                            return 1;
                        } elseif ($constType->isFloat()) {
                            return M_PI;
                        } elseif ($constType->isString()) {
                            return $const->name;
                        } elseif ($constType->isArray()) {
                            return [];
                        }
                    }

                    return null;
                }

                throw new Exception("Constant " . $originatingConstName->__toString() . " cannot be found");
            }
        );

        $result = $evaluator->evaluateDirectly($expr);

        return new EvaluatedValue(
            $result, // note: we are generally not interested in the actual value of $result, unless it's a bare value, without constants
            $constType ?? SimpleType::fromValue($result),
            $cConstName === null ? $expr : new Expr\ConstFetch(new Node\Name($cConstName)),
            $visitor->visitedConstants,
            $isUnknownConstValue
        );
    }

    public static function null(): EvaluatedValue
    {
        return new self(null, SimpleType::null(), new Expr\ConstFetch(new Node\Name('null')), [], false);
    }

    /**
     * @param mixed $value
     * @param ConstInfo[] $originatingConsts
     */
    private function __construct($value, SimpleType $type, Expr $expr, array $originatingConsts, bool $isUnknownConstValue)
    {
        $this->value = $value;
        $this->type = $type;
        $this->expr = $expr;
        $this->originatingConsts = $originatingConsts;
        $this->isUnknownConstValue = $isUnknownConstValue;
    }

    public function initializeZval(string $zvalName): string
    {
        $cExpr = $this->getCExpr();

        $code = "\tzval $zvalName;\n";

        if ($this->type->isNull()) {
            $code .= "\tZVAL_NULL(&$zvalName);\n";
        } elseif ($this->type->isBool()) {
            if ($cExpr == 'true') {
                $code .= "\tZVAL_TRUE(&$zvalName);\n";
            } elseif ($cExpr == 'false') {
                $code .= "\tZVAL_FALSE(&$zvalName);\n";
            } else {
                $code .= "\tZVAL_BOOL(&$zvalName, $cExpr);\n";
            }
        } elseif ($this->type->isInt()) {
            $code .= "\tZVAL_LONG(&$zvalName, $cExpr);\n";
        } elseif ($this->type->isFloat()) {
            $code .= "\tZVAL_DOUBLE(&$zvalName, $cExpr);\n";
        } elseif ($this->type->isString()) {
            if ($cExpr === '""') {
                $code .= "\tZVAL_EMPTY_STRING(&$zvalName);\n";
            } else {
                $code .= "\tzend_string *{$zvalName}_str = zend_string_init($cExpr, strlen($cExpr), 1);\n";
                $code .= "\tZVAL_STR(&$zvalName, {$zvalName}_str);\n";
            }
        } elseif ($this->type->isArray()) {
            if ($cExpr == '[]') {
                $code .= "\tZVAL_EMPTY_ARRAY(&$zvalName);\n";
            } else {
                throw new Exception("Unimplemented default value");
            }
        } else {
            throw new Exception("Invalid default value: " . print_r($this->value, true) . ", type: " . print_r($this->type, true));
        }

        return $code;
    }

    public function getCExpr(): ?string
    {
        // $this->expr has all its PHP constants replaced by C constants
        $prettyPrinter = new Standard;
        $expr = $prettyPrinter->prettyPrintExpr($this->expr);
        // PHP single-quote to C double-quote string
        if ($this->type->isString()) {
            $expr = preg_replace("/(^'|'$)/", '"', $expr);
        }
        return $expr[0] == '"' ? $expr : preg_replace('(\bnull\b)', 'NULL', str_replace('\\', '', $expr));
    }
}

abstract class VariableLike
{
    public int $flags;
    public ?Type $type;
    public ?Type $phpDocType;
    public ?string $link;
    public ?int $phpVersionIdMinimumCompatibility;
    /** @var AttributeInfo[] */
    public array $attributes;

    /**
     * @var AttributeInfo[] $attributes
     */
    public function __construct(
        int $flags,
        ?Type $type,
        ?Type $phpDocType,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes
    ) {
        $this->flags = $flags;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->link = $link;
        $this->phpVersionIdMinimumCompatibility = $phpVersionIdMinimumCompatibility;
        $this->attributes = $attributes;
    }

    abstract protected function getVariableTypeCode(): string;

    abstract protected function getVariableTypeName(): string;

    abstract protected function getFieldSynopsisDefaultLinkend(): string;

    abstract protected function getFieldSynopsisName(): string;

    /** @param array<string, ConstInfo> $allConstInfos */
    abstract protected function getFieldSynopsisValueString(array $allConstInfos): ?string;

    abstract public function discardInfoForOldPhpVersions(): void;

    protected function addTypeToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        $type = $this->phpDocType ?? $this->type;

        if ($type) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($type->getTypeForDoc($doc));
        }
    }

    /**
     * @return array<int, string[]>
     */
    protected function getFlagsByPhpVersion(): array
    {
        $flags = "ZEND_ACC_PUBLIC";
        if ($this->flags & Class_::MODIFIER_PROTECTED) {
            $flags = "ZEND_ACC_PROTECTED";
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $flags = "ZEND_ACC_PRIVATE";
        }

        return [
            PHP_70_VERSION_ID => [$flags],
            PHP_80_VERSION_ID => [$flags],
            PHP_81_VERSION_ID => [$flags],
            PHP_82_VERSION_ID => [$flags],
            PHP_83_VERSION_ID => [$flags],
        ];
    }

    protected function getTypeCode(string $variableLikeName, string &$code): string
    {
        $variableLikeType = $this->getVariableTypeName();

        $typeCode = "";
        if ($this->type) {
            $arginfoType = $this->type->toArginfoType();
            if ($arginfoType->hasClassType()) {
                if (count($arginfoType->classTypes) >= 2) {
                    foreach ($arginfoType->classTypes as $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $varEscapedClassName = $classType->toVarEscapedName();
                        $code .= "\tzend_string *{$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"{$escapedClassName}\") - 1, 1);\n";
                    }

                    $classTypeCount = count($arginfoType->classTypes);
                    $code .= "\tzend_type_list *{$variableLikeType}_{$variableLikeName}_type_list = malloc(ZEND_TYPE_LIST_SIZE($classTypeCount));\n";
                    $code .= "\t{$variableLikeType}_{$variableLikeName}_type_list->num_types = $classTypeCount;\n";

                    foreach ($arginfoType->classTypes as $k => $classType) {
                        $escapedClassName = $classType->toEscapedName();
                        $code .= "\t{$variableLikeType}_{$variableLikeName}_type_list->types[$k] = (zend_type) ZEND_TYPE_INIT_CLASS({$variableLikeType}_{$variableLikeName}_class_{$escapedClassName}, 0, 0);\n";
                    }

                    $typeMaskCode = $this->type->toArginfoType()->toTypeMask();

                    if ($this->type->isIntersection) {
                        $code .= "\tzend_type {$variableLikeType}_{$variableLikeName}_type = ZEND_TYPE_INIT_INTERSECTION({$variableLikeType}_{$variableLikeName}_type_list, $typeMaskCode);\n";
                    } else {
                        $code .= "\tzend_type {$variableLikeType}_{$variableLikeName}_type = ZEND_TYPE_INIT_UNION({$variableLikeType}_{$variableLikeName}_type_list, $typeMaskCode);\n";
                    }
                    $typeCode = "{$variableLikeType}_{$variableLikeName}_type";
                } else {
                    $escapedClassName = $arginfoType->classTypes[0]->toEscapedName();
                    $varEscapedClassName = $arginfoType->classTypes[0]->toVarEscapedName();
                    $code .= "\tzend_string *{$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName} = zend_string_init(\"{$escapedClassName}\", sizeof(\"{$escapedClassName}\")-1, 1);\n";

                    $typeCode = "(zend_type) ZEND_TYPE_INIT_CLASS({$variableLikeType}_{$variableLikeName}_class_{$varEscapedClassName}, 0, " . $arginfoType->toTypeMask() . ")";
                }
            } else {
                $typeCode = "(zend_type) ZEND_TYPE_INIT_MASK(" . $arginfoType->toTypeMask() . ")";
            }
        } else {
            $typeCode = "(zend_type) ZEND_TYPE_INIT_NONE(0)";
        }

        return $typeCode;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getFieldSynopsisElement(DOMDocument $doc, array $allConstInfos): DOMElement
    {
        $fieldsynopsisElement = $doc->createElement("fieldsynopsis");

        $this->addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        $this->addTypeToFieldSynopsis($doc, $fieldsynopsisElement);

        $varnameElement = $doc->createElement("varname", $this->getFieldSynopsisName());
        if ($this->link) {
            $varnameElement->setAttribute("linkend", $this->link);
        } else {
            $varnameElement->setAttribute("linkend", $this->getFieldSynopsisDefaultLinkend());
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($varnameElement);

        $valueString = $this->getFieldSynopsisValueString($allConstInfos);
        if ($valueString) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $initializerElement = $doc->createElement("initializer",  $valueString);
            $fieldsynopsisElement->appendChild($initializerElement);
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n    "));

        return $fieldsynopsisElement;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        if ($this->flags & Class_::MODIFIER_PUBLIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "public"));
        } elseif ($this->flags & Class_::MODIFIER_PROTECTED) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "protected"));
        } elseif ($this->flags & Class_::MODIFIER_PRIVATE) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "private"));
        }
    }

    /**
     * @param array<int, string[]> $flags
     * @return array<int, string[]>
     */
    protected function addFlagForVersionsAbove(array $flags, string $flag, int $minimumVersionId): array
    {
        $write = false;

        foreach ($flags as $version => $versionFlags) {
            if ($version === $minimumVersionId || $write === true) {
                $flags[$version][] = $flag;
                $write = true;
            }
        }

        return $flags;
    }
}

class ConstInfo extends VariableLike
{
    public ConstOrClassConstName $name;
    public Expr $value;
    public bool $isDeprecated;
    public ?string $valueString;
    public ?string $cond;
    public ?string $cValue;
    public bool $isUndocumentable;

    /**
     * @var AttributeInfo[] $attributes
     */
    public function __construct(
        ConstOrClassConstName $name,
        int $flags,
        Expr $value,
        ?string $valueString,
        ?Type $type,
        ?Type $phpDocType,
        bool $isDeprecated,
        ?string $cond,
        ?string $cValue,
        bool $isUndocumentable,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes
    ) {
        $this->name = $name;
        $this->value = $value;
        $this->valueString = $valueString;
        $this->isDeprecated = $isDeprecated;
        $this->cond = $cond;
        $this->cValue = $cValue;
        $this->isUndocumentable = $isUndocumentable;
        parent::__construct($flags, $type, $phpDocType, $link, $phpVersionIdMinimumCompatibility, $attributes);
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getValue(array $allConstInfos): EvaluatedValue
    {
        return EvaluatedValue::createFromExpression(
            $this->value,
            ($this->phpDocType ?? $this->type)->tryToSimpleType(),
            $this->cValue,
            $allConstInfos
        );
    }

    protected function getVariableTypeName(): string
    {
        return "constant";
    }

    protected function getVariableTypeCode(): string
    {
        return "const";
    }

    protected function getFieldSynopsisDefaultLinkend(): string
    {
        $className = str_replace(["\\", "_"], ["-", "-"], $this->name->class->toLowerString());

        return "$className.constants." . strtolower(str_replace("_", "-", $this->name->getDeclarationName()));
    }

    protected function getFieldSynopsisName(): string
    {
        return $this->name->__toString();
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    protected function getFieldSynopsisValueString(array $allConstInfos): ?string
    {
        $value = EvaluatedValue::createFromExpression($this->value, null, $this->cValue, $allConstInfos);
        if ($value->isUnknownConstValue) {
            return null;
        }

        if ($value->originatingConsts) {
            return implode("\n", array_map(function (ConstInfo $const) use ($allConstInfos) {
                return $const->getFieldSynopsisValueString($allConstInfos);
            }, $value->originatingConsts));
        }

        return $this->valueString;
    }

    public function getPredefinedConstantTerm(DOMDocument $doc, int $indentationLevel): DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        $termElement = $doc->createElement("term");

        $constantElement = $doc->createElement("constant");
        $constantElement->textContent = $this->name->__toString();

        $typeElement = ($this->phpDocType ?? $this->type)->getTypeForDoc($doc);

        $termElement->appendChild(new DOMText("\n$indentation "));
        $termElement->appendChild($constantElement);
        $termElement->appendChild(new DOMText("\n$indentation ("));
        $termElement->appendChild($typeElement);
        $termElement->appendChild(new DOMText(")\n$indentation"));

        return $termElement;
    }

     public function getPredefinedConstantEntry(DOMDocument $doc, int $indentationLevel): DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        $entryElement = $doc->createElement("entry");

        $constantElement = $doc->createElement("constant");
        $constantElement->textContent = $this->name->__toString();
        $typeElement = ($this->phpDocType ?? $this->type)->getTypeForDoc($doc);

        $entryElement->appendChild(new DOMText("\n$indentation "));
        $entryElement->appendChild($constantElement);
        $entryElement->appendChild(new DOMText("\n$indentation ("));
        $entryElement->appendChild($typeElement);
        $entryElement->appendChild(new DOMText(")\n$indentation"));

        return $entryElement;
    }

    public function discardInfoForOldPhpVersions(): void {
        $this->type = null;
        $this->flags &= ~Class_::MODIFIER_FINAL;
        $this->isDeprecated = false;
        $this->attributes = [];
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string
    {
        $simpleType = ($this->phpDocType ?? $this->type)->tryToSimpleType();
        if ($simpleType && $simpleType->name === "mixed") {
            $simpleType = null;
        }

        $value = EvaluatedValue::createFromExpression($this->value, $simpleType, $this->cValue, $allConstInfos);
        if ($value->isUnknownConstValue && ($simpleType === null || !$simpleType->isBuiltin)) {
            throw new Exception("Constant " . $this->name->__toString() . " must have a built-in PHPDoc type as the type couldn't be inferred from its value");
        }

        // i.e. const NAME = UNKNOWN;, without the annotation
        if ($value->isUnknownConstValue && $this->cValue === null && $value->expr instanceof Expr\ConstFetch && $value->expr->name->__toString() === "UNKNOWN") {
            throw new Exception("Constant " . $this->name->__toString() . " must have a @cvalue annotation");
        }

        $code = "";

        if ($this->cond) {
            $code .= "#if {$this->cond}\n";
        }

        if ($this->name->isClassConst()) {
            $code .= $this->getClassConstDeclaration($value, $allConstInfos);
        } else {
            $code .= $this->getGlobalConstDeclaration($value, $allConstInfos);
        }
        $code .= $this->getValueAssertion($value);

        if ($this->cond) {
            $code .= "#endif\n";
        }

        return $code;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    private function getGlobalConstDeclaration(EvaluatedValue $value, array $allConstInfos): string
    {
        $constName = str_replace('\\', '\\\\', $this->name->__toString());
        $constValue = $value->value;
        $cExpr = $value->getCExpr();

        $flags = "CONST_PERSISTENT";
        if ($this->phpVersionIdMinimumCompatibility !== null && $this->phpVersionIdMinimumCompatibility < 80000) {
            $flags .= " | CONST_CS";
        }

        if ($this->isDeprecated) {
            $flags .= " | CONST_DEPRECATED";
        }
        if ($value->type->isNull()) {
            return "\tREGISTER_NULL_CONSTANT(\"$constName\", $flags);\n";
        }

        if ($value->type->isBool()) {
            return "\tREGISTER_BOOL_CONSTANT(\"$constName\", " . ($cExpr ?: ($constValue ? "true" : "false")) . ", $flags);\n";
        }

        if ($value->type->isInt()) {
            return "\tREGISTER_LONG_CONSTANT(\"$constName\", " . ($cExpr ?: (int) $constValue) . ", $flags);\n";
        }

        if ($value->type->isFloat()) {
            return "\tREGISTER_DOUBLE_CONSTANT(\"$constName\", " . ($cExpr ?: (float) $constValue) . ", $flags);\n";
        }

        if ($value->type->isString()) {
            return "\tREGISTER_STRING_CONSTANT(\"$constName\", " . ($cExpr ?: '"' . addslashes($constValue) . '"') . ", $flags);\n";
        }

        throw new Exception("Unimplemented constant type");}

    /** @param array<string, ConstInfo> $allConstInfos */
    private function getClassConstDeclaration(EvaluatedValue $value, array $allConstInfos): string
    {
        $constName = $this->name->getDeclarationName();

        $zvalCode = $value->initializeZval("const_{$constName}_value", $allConstInfos);

        $code = "\n" . $zvalCode;

        $code .= "\tzend_string *const_{$constName}_name = zend_string_init_interned(\"$constName\", sizeof(\"$constName\") - 1, 1);\n";
        $nameCode = "const_{$constName}_name";

        $php83MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_83_VERSION_ID;

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#if (PHP_VERSION_ID >= " . PHP_83_VERSION_ID . ")\n";
        }

        if ($this->type) {
            $typeCode = $this->getTypeCode($constName, $code);

            if (!empty($this->attributes)) {
                $template = "\tzend_class_constant *const_" . $this->name->getDeclarationName() . " = ";
            } else {
                $template = "\t";
            }
            $template .= "zend_declare_typed_class_constant(class_entry, $nameCode, &const_{$constName}_value, %s, NULL, $typeCode);\n";

            $flagsCode = generateVersionDependentFlagCode(
                $template,
                $this->getFlagsByPhpVersion(),
                $this->phpVersionIdMinimumCompatibility
            );
            $code .= implode("", $flagsCode);
        }

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#else\n";
        }

        if (!$this->type || !$php83MinimumCompatibility) {
            if (!empty($this->attributes)) {
                $template = "\tzend_class_constant *const_" . $this->name->getDeclarationName() . " = ";
            } else {
                $template = "\t";
            }
            $template .= "zend_declare_class_constant_ex(class_entry, $nameCode, &const_{$constName}_value, %s, NULL);\n";
            $flagsCode = generateVersionDependentFlagCode(
                $template,
                $this->getFlagsByPhpVersion(),
                $this->phpVersionIdMinimumCompatibility
            );
            $code .= implode("", $flagsCode);
        }

        if ($this->type && !$php83MinimumCompatibility) {
            $code .= "#endif\n";
        }

        $code .= "\tzend_string_release(const_{$constName}_name);\n";

        return $code;
    }

    private function getValueAssertion(EvaluatedValue $value): string
    {
        if ($value->isUnknownConstValue || $value->originatingConsts || $this->cValue === null) {
            return "";
        }

        $cExpr = $value->getCExpr();
        $constValue = $value->value;

        if ($value->type->isNull()) {
            return "\tZEND_ASSERT($cExpr == NULL);\n";
        }

        if ($value->type->isBool()) {
            $cValue = $constValue ? "true" : "false";
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isInt()) {
            $cValue = (int) $constValue;
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isFloat()) {
            $cValue = (float) $constValue;
            return "\tZEND_ASSERT($cExpr == $cValue);\n";
        }

        if ($value->type->isString()) {
            $cValue = '"' . addslashes($constValue) . '"';
            return "\tZEND_ASSERT(strcmp($cExpr, $cValue) == 0);\n";
        }

        throw new Exception("Unimplemented constant type");
    }

    /**
     * @return array<int, string[]>
     */
    protected function getFlagsByPhpVersion(): array
    {
        $flags = parent::getFlagsByPhpVersion();

        if ($this->isDeprecated) {
            $flags = $this->addFlagForVersionsAbove($flags, "ZEND_ACC_DEPRECATED", PHP_80_VERSION_ID);
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $flags = $this->addFlagForVersionsAbove($flags, "ZEND_ACC_FINAL", PHP_81_VERSION_ID);
        }

        return $flags;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        parent::addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "final"));
        }

        $fieldsynopsisElement->appendChild(new DOMText("\n     "));
        $fieldsynopsisElement->appendChild($doc->createElement("modifier", "const"));
    }
}

class PropertyInfo extends VariableLike
{
    public PropertyName $name;
    public ?Expr $defaultValue;
    public ?string $defaultValueString;
    public bool $isDocReadonly;

    /**
     * @var AttributeInfo[] $attributes
     */
    public function __construct(
        PropertyName $name,
        int $flags,
        ?Type $type,
        ?Type $phpDocType,
        ?Expr $defaultValue,
        ?string $defaultValueString,
        bool $isDocReadonly,
        ?string $link,
        ?int $phpVersionIdMinimumCompatibility,
        array $attributes
    ) {
        $this->name = $name;
        $this->defaultValue = $defaultValue;
        $this->defaultValueString = $defaultValueString;
        $this->isDocReadonly = $isDocReadonly;
        parent::__construct($flags, $type, $phpDocType, $link, $phpVersionIdMinimumCompatibility, $attributes);
    }

    protected function getVariableTypeCode(): string
    {
        return "property";
    }

    protected function getVariableTypeName(): string
    {
        return "property";
    }

    protected function getFieldSynopsisDefaultLinkend(): string
    {
        $className = str_replace(["\\", "_"], ["-", "-"], $this->name->class->toLowerString());

        return "$className.props." . strtolower(str_replace("_", "-", $this->name->getDeclarationName()));
    }

    protected function getFieldSynopsisName(): string
    {
        return $this->name->getDeclarationName();
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    protected function getFieldSynopsisValueString(array $allConstInfos): ?string
    {
        return $this->defaultValueString;
    }

    public function discardInfoForOldPhpVersions(): void {
        $this->type = null;
        $this->flags &= ~Class_::MODIFIER_READONLY;
        $this->attributes = [];
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string {
        $code = "\n";

        $propertyName = $this->name->getDeclarationName();

        if ($this->defaultValue === null) {
            $defaultValue = EvaluatedValue::null();
        } else {
            $defaultValue = EvaluatedValue::createFromExpression($this->defaultValue, null, null, $allConstInfos);
            if ($defaultValue->isUnknownConstValue || ($defaultValue->originatingConsts && $defaultValue->getCExpr() === null)) {
                echo "Skipping code generation for property $this->name, because it has an unknown constant default value\n";
                return "";
            }
        }

        $zvalName = "property_{$propertyName}_default_value";
        if ($this->defaultValue === null && $this->type !== null) {
            $code .= "\tzval $zvalName;\n\tZVAL_UNDEF(&$zvalName);\n";
        } else {
            $code .= $defaultValue->initializeZval($zvalName);
        }

        $code .= "\tzend_string *property_{$propertyName}_name = zend_string_init(\"$propertyName\", sizeof(\"$propertyName\") - 1, 1);\n";
        $nameCode = "property_{$propertyName}_name";
        $typeCode = $this->getTypeCode($propertyName, $code);

        if (!empty($this->attributes)) {
            $template = "\tzend_property_info *property_" . $this->name->getDeclarationName() . " = ";
        } else {
            $template = "\t";
        }
        $template .= "zend_declare_typed_property(class_entry, $nameCode, &$zvalName, %s, NULL, $typeCode);\n";
        $flagsCode = generateVersionDependentFlagCode(
            $template,
            $this->getFlagsByPhpVersion(),
            $this->phpVersionIdMinimumCompatibility
        );
        $code .= implode("", $flagsCode);

        $code .= "\tzend_string_release(property_{$propertyName}_name);\n";

        return $code;
    }

    /**
     * @return array<int, string[]>
     */
    protected function getFlagsByPhpVersion(): array
    {
        $flags = parent::getFlagsByPhpVersion();

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $flags = $this->addFlagForVersionsAbove($flags, "ZEND_ACC_STATIC", PHP_70_VERSION_ID);
        }

        if ($this->flags & Class_::MODIFIER_READONLY) {
            $flags = $this->addFlagForVersionsAbove($flags, "ZEND_ACC_READONLY", PHP_81_VERSION_ID);
        }

        return $flags;
    }

    protected function addModifiersToFieldSynopsis(DOMDocument $doc, DOMElement $fieldsynopsisElement): void
    {
        parent::addModifiersToFieldSynopsis($doc, $fieldsynopsisElement);

        if ($this->flags & Class_::MODIFIER_STATIC) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "static"));
        }

        if ($this->flags & Class_::MODIFIER_READONLY || $this->isDocReadonly) {
            $fieldsynopsisElement->appendChild(new DOMText("\n     "));
            $fieldsynopsisElement->appendChild($doc->createElement("modifier", "readonly"));
        }
    }

    public function __clone()
    {
        if ($this->type) {
            $this->type = clone $this->type;
        }
    }
}

class EnumCaseInfo {
    public string $name;
    public ?Expr $value;

    public function __construct(string $name, ?Expr $value) {
        $this->name = $name;
        $this->value = $value;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getDeclaration(array $allConstInfos): string {
        $escapedName = addslashes($this->name);
        if ($this->value === null) {
            $code = "\n\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", NULL);\n";
        } else {
            $value = EvaluatedValue::createFromExpression($this->value, null, null, $allConstInfos);

            $zvalName = "enum_case_{$escapedName}_value";
            $code = "\n" . $value->initializeZval($zvalName);
            $code .= "\tzend_enum_add_case_cstr(class_entry, \"$escapedName\", &$zvalName);\n";
        }

        return $code;
    }
}

class AttributeInfo {
    public string $class;
    /** @var \PhpParser\Node\Arg[] */
    public array $args;

    /** @param \PhpParser\Node\Arg[] $args */
    public function __construct(string $class, array $args) {
        $this->class = $class;
        $this->args = $args;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function generateCode(string $invocation, string $nameSuffix, array $allConstInfos, ?int $phpVersionIdMinimumCompatibility): string {
        $php82MinimumCompatibility = $phpVersionIdMinimumCompatibility === null || $phpVersionIdMinimumCompatibility >= PHP_82_VERSION_ID;
        /* see ZEND_KNOWN_STRINGS in Zend/strings.h */
        $knowns = [];
        if ($php82MinimumCompatibility) {
            $knowns["SensitiveParameter"] = "ZEND_STR_SENSITIVEPARAMETER";
        }

        $code = "\n";
        $escapedAttributeName = strtr($this->class, '\\', '_');
        if (isset($knowns[$escapedAttributeName])) {
            $code .= "\t" . ($this->args ? "zend_attribute *attribute_{$escapedAttributeName}_$nameSuffix = " : "") . "$invocation, ZSTR_KNOWN({$knowns[$escapedAttributeName]}), " . count($this->args) . ");\n";
        } else {
            $code .= "\tzend_string *attribute_name_{$escapedAttributeName}_$nameSuffix = zend_string_init_interned(\"" . addcslashes($this->class, "\\") . "\", sizeof(\"" . addcslashes($this->class, "\\") . "\") - 1, 1);\n";
            $code .= "\t" . ($this->args ? "zend_attribute *attribute_{$escapedAttributeName}_$nameSuffix = " : "") . "$invocation, attribute_name_{$escapedAttributeName}_$nameSuffix, " . count($this->args) . ");\n";
            $code .= "\tzend_string_release(attribute_name_{$escapedAttributeName}_$nameSuffix);\n";
        }
        foreach ($this->args as $i => $arg) {
            $value = EvaluatedValue::createFromExpression($arg->value, null, null, $allConstInfos);
            $zvalName = "attribute_{$escapedAttributeName}_{$nameSuffix}_arg$i";
            $code .= $value->initializeZval($zvalName);
            $code .= "\tZVAL_COPY_VALUE(&attribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].value, &$zvalName);\n";
            if ($arg->name) {
                $code .= "\tattribute_{$escapedAttributeName}_{$nameSuffix}->args[$i].name = zend_string_init(\"{$arg->name->name}\", sizeof(\"{$arg->name->name}\") - 1, 1);\n";
            }
        }
        return $code;
    }
}

class ClassInfo {
    public Name $name;
    public int $flags;
    public string $type;
    public ?string $alias;
    public ?SimpleType $enumBackingType;
    public bool $isDeprecated;
    public bool $isStrictProperties;
    /** @var AttributeInfo[] */
    public array $attributes;
    public bool $isNotSerializable;
    /** @var Name[] */
    public array $extends;
    /** @var Name[] */
    public array $implements;
    /** @var ConstInfo[] */
    public array $constInfos;
    /** @var PropertyInfo[] */
    public array $propertyInfos;
    /** @var FuncInfo[] */
    public array $funcInfos;
    /** @var EnumCaseInfo[] */
    public array $enumCaseInfos;
    public ?string $cond;
    public ?int $phpVersionIdMinimumCompatibility;
    public bool $isUndocumentable;

    /**
     * @param AttributeInfo[] $attributes
     * @param Name[] $extends
     * @param Name[] $implements
     * @param ConstInfo[] $constInfos
     * @param PropertyInfo[] $propertyInfos
     * @param FuncInfo[] $funcInfos
     * @param EnumCaseInfo[] $enumCaseInfos
     */
    public function __construct(
        Name $name,
        int $flags,
        string $type,
        ?string $alias,
        ?SimpleType $enumBackingType,
        bool $isDeprecated,
        bool $isStrictProperties,
        array $attributes,
        bool $isNotSerializable,
        array $extends,
        array $implements,
        array $constInfos,
        array $propertyInfos,
        array $funcInfos,
        array $enumCaseInfos,
        ?string $cond,
        ?int $minimumPhpVersionIdCompatibility,
        bool $isUndocumentable
    ) {
        $this->name = $name;
        $this->flags = $flags;
        $this->type = $type;
        $this->alias = $alias;
        $this->enumBackingType = $enumBackingType;
        $this->isDeprecated = $isDeprecated;
        $this->isStrictProperties = $isStrictProperties;
        $this->attributes = $attributes;
        $this->isNotSerializable = $isNotSerializable;
        $this->extends = $extends;
        $this->implements = $implements;
        $this->constInfos = $constInfos;
        $this->propertyInfos = $propertyInfos;
        $this->funcInfos = $funcInfos;
        $this->enumCaseInfos = $enumCaseInfos;
        $this->cond = $cond;
        $this->phpVersionIdMinimumCompatibility = $minimumPhpVersionIdCompatibility;
        $this->isUndocumentable = $isUndocumentable;
    }

    /** @param array<string, ConstInfo> $allConstInfos */
    public function getRegistration(array $allConstInfos): string
    {
        $params = [];
        foreach ($this->extends as $extends) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $extends->getParts());
        }
        foreach ($this->implements as $implements) {
            $params[] = "zend_class_entry *class_entry_" . implode("_", $implements->getParts());
        }

        $escapedName = implode("_", $this->name->getParts());

        $code = '';

        $php80MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_80_VERSION_ID;
        $php81MinimumCompatibility = $this->phpVersionIdMinimumCompatibility === null || $this->phpVersionIdMinimumCompatibility >= PHP_81_VERSION_ID;

        if ($this->type === "enum" && !$php81MinimumCompatibility) {
            $code .= "#if (PHP_VERSION_ID >= " . PHP_81_VERSION_ID . ")\n";
        }

        if ($this->cond) {
            $code .= "#if {$this->cond}\n";
        }

        $code .= "static zend_class_entry *register_class_$escapedName(" . (empty($params) ? "void" : implode(", ", $params)) . ")\n";

        $code .= "{\n";
        if ($this->type === "enum") {
            $name = addslashes((string) $this->name);
            $backingType = $this->enumBackingType
                ? $this->enumBackingType->toTypeCode() : "IS_UNDEF";
            $code .= "\tzend_class_entry *class_entry = zend_register_internal_enum(\"$name\", $backingType, class_{$escapedName}_methods);\n";
        } else {
            $code .= "\tzend_class_entry ce, *class_entry;\n\n";
            if (count($this->name->getParts()) > 1) {
                $className = $this->name->getLast();
                $namespace = addslashes((string) $this->name->slice(0, -1));

                $code .= "\tINIT_NS_CLASS_ENTRY(ce, \"$namespace\", \"$className\", class_{$escapedName}_methods);\n";
            } else {
                $code .= "\tINIT_CLASS_ENTRY(ce, \"$this->name\", class_{$escapedName}_methods);\n";
            }

            if ($this->type === "class" || $this->type === "trait") {
                $code .= "\tclass_entry = zend_register_internal_class_ex(&ce, " . (isset($this->extends[0]) ? "class_entry_" . str_replace("\\", "_", $this->extends[0]->toString()) : "NULL") . ");\n";
            } else {
                $code .= "\tclass_entry = zend_register_internal_interface(&ce);\n";
            }
        }

        $flagCodes = generateVersionDependentFlagCode("\tclass_entry->ce_flags |= %s;\n", $this->getFlagsByPhpVersion(), $this->phpVersionIdMinimumCompatibility);
        $code .= implode("", $flagCodes);

        $implements = array_map(
            function (Name $item) {
                return "class_entry_" . implode("_", $item->getParts());
            },
            $this->type === "interface" ? $this->extends : $this->implements
        );

        if (!empty($implements)) {
            $code .= "\tzend_class_implements(class_entry, " . count($implements) . ", " . implode(", ", $implements) . ");\n";
        }

        if ($this->alias) {
            $code .= "\tzend_register_class_alias(\"" . str_replace("\\", "\\\\", $this->alias) . "\", class_entry);\n";
        }

        foreach ($this->constInfos as $const) {
            $code .= $const->getDeclaration($allConstInfos);
        }

        foreach ($this->enumCaseInfos as $enumCase) {
            $code .= $enumCase->getDeclaration($allConstInfos);
        }

        foreach ($this->propertyInfos as $property) {
            $code .= $property->getDeclaration($allConstInfos);
        }

        if (!empty($this->attributes)) {
            if (!$php80MinimumCompatibility) {
                $code .= "\n#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")";
            }

            foreach ($this->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_class_attribute(class_entry",
                    "class_{$escapedName}_$key",
                    $allConstInfos,
                    $this->phpVersionIdMinimumCompatibility
                );
            }

            if (!$php80MinimumCompatibility) {
                $code .= "#endif\n";
            }
        }

        if ($attributeInitializationCode = generateConstantAttributeInitialization($this->constInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility, $this->cond)) {
            if (!$php80MinimumCompatibility) {
                $code .= "#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")";
            }

            $code .= "\n" . $attributeInitializationCode;

            if (!$php80MinimumCompatibility) {
                $code .= "#endif\n";
            }
        }

        if ($attributeInitializationCode = generatePropertyAttributeInitialization($this->propertyInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility)) {
            if (!$php80MinimumCompatibility) {
                $code .= "#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")";
            }

            $code .= "\n" . $attributeInitializationCode;

            if (!$php80MinimumCompatibility) {
                $code .= "#endif\n";
            }
        }

        if ($attributeInitializationCode = generateFunctionAttributeInitialization($this->funcInfos, $allConstInfos, $this->phpVersionIdMinimumCompatibility, $this->cond)) {
            if (!$php80MinimumCompatibility) {
                $code .= "#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")\n";
            }

            $code .= "\n" . $attributeInitializationCode;

            if (!$php80MinimumCompatibility) {
                $code .= "#endif\n";
            }
        }

        $code .= "\n\treturn class_entry;\n";

        $code .= "}\n";

        if ($this->cond) {
            $code .= "#endif\n";
        }

        if ($this->type === "enum" && !$php81MinimumCompatibility) {
            $code .= "#endif\n";
        }

        return $code;
    }

    /**
     * @return array<int, string[]>
     */
    private function getFlagsByPhpVersion(): array
    {
        $php70Flags = [];

        if ($this->type === "trait") {
            $php70Flags[] = "ZEND_ACC_TRAIT";
        }

        if ($this->flags & Class_::MODIFIER_FINAL) {
            $php70Flags[] = "ZEND_ACC_FINAL";
        }

        if ($this->flags & Class_::MODIFIER_ABSTRACT) {
            $php70Flags[] = "ZEND_ACC_ABSTRACT";
        }

        if ($this->isDeprecated) {
            $php70Flags[] = "ZEND_ACC_DEPRECATED";
        }

        $php80Flags = $php70Flags;

        if ($this->isStrictProperties) {
            $php80Flags[] = "ZEND_ACC_NO_DYNAMIC_PROPERTIES";
        }

        $php81Flags = $php80Flags;

        if ($this->isNotSerializable) {
            $php81Flags[] = "ZEND_ACC_NOT_SERIALIZABLE";
        }

        $php82Flags = $php81Flags;

        if ($this->flags & Class_::MODIFIER_READONLY) {
            $php82Flags[] = "ZEND_ACC_READONLY_CLASS";
        }

        foreach ($this->attributes as $attr) {
            if ($attr->class === "AllowDynamicProperties") {
                $php82Flags[] = "ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES";
                break;
            }
        }

        $php83Flags = $php82Flags;

        return [
            PHP_70_VERSION_ID => $php70Flags,
            PHP_80_VERSION_ID => $php80Flags,
            PHP_81_VERSION_ID => $php81Flags,
            PHP_82_VERSION_ID => $php82Flags,
            PHP_83_VERSION_ID => $php83Flags,
        ];
    }

    /**
     * @param array<string, ClassInfo> $classMap
     * @param array<string, ConstInfo> $allConstInfos
     * @param iterable<ConstInfo> $allConstInfo
     */
    public function getClassSynopsisDocument(array $classMap, array $allConstInfos): ?string {

        $doc = new DOMDocument();
        $doc->formatOutput = true;
        $classSynopsis = $this->getClassSynopsisElement($doc, $classMap, $allConstInfos);
        if (!$classSynopsis) {
            return null;
        }

        $doc->appendChild($classSynopsis);

        return $doc->saveXML();
    }

    /**
     * @param array<string, ClassInfo> $classMap
     * @param array<string, ConstInfo> $allConstInfos
     */
    public function getClassSynopsisElement(DOMDocument $doc, array $classMap, array $allConstInfos): ?DOMElement {

        $classSynopsis = $doc->createElement("classsynopsis");
        $classSynopsis->setAttribute("class", $this->type === "interface" ? "interface" : "class");

        $exceptionOverride = $this->type === "class" && $this->isException($classMap) ? "exception" : null;
        $ooElement = self::createOoElement($doc, $this, $exceptionOverride, true, null, 4);
        if (!$ooElement) {
            return null;
        }
        $classSynopsis->appendChild(new DOMText("\n    "));
        $classSynopsis->appendChild($ooElement);

        foreach ($this->extends as $k => $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if ($parentInfo === null) {
                throw new Exception("Missing parent class " . $parent->toString());
            }

            $ooElement = self::createOoElement(
                $doc,
                $parentInfo,
                null,
                false,
                $k === 0 ? "extends" : null,
                4
            );
            if (!$ooElement) {
                return null;
            }

            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsis->appendChild($ooElement);
        }

        foreach ($this->implements as $k => $interface) {
            $interfaceInfo = $classMap[$interface->toString()] ?? null;
            if (!$interfaceInfo) {
                throw new Exception("Missing implemented interface " . $interface->toString());
            }

            $ooElement = self::createOoElement($doc, $interfaceInfo, null, false, $k === 0 ? "implements" : null, 4);
            if (!$ooElement) {
                return null;
            }
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsis->appendChild($ooElement);
        }

        /** @var array<string, Name> $parentsWithInheritedConstants */
        $parentsWithInheritedConstants = [];
        /** @var array<string, Name> $parentsWithInheritedProperties */
        $parentsWithInheritedProperties = [];
        /** @var array<int, array{name: Name, types: int[]}> $parentsWithInheritedMethods */
        $parentsWithInheritedMethods = [];

        $this->collectInheritedMembers(
            $parentsWithInheritedConstants,
            $parentsWithInheritedProperties,
            $parentsWithInheritedMethods,
            $this->hasConstructor(),
            $classMap
        );

        $this->appendInheritedMemberSectionToClassSynopsis(
            $doc,
            $classSynopsis,
            $parentsWithInheritedConstants,
            "&Constants;",
            "&InheritedConstants;"
        );

        if (!empty($this->constInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Constants;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($this->constInfos as $constInfo) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $fieldSynopsisElement = $constInfo->getFieldSynopsisElement($doc, $allConstInfos);
                $classSynopsis->appendChild($fieldSynopsisElement);
            }
        }

        if (!empty($this->propertyInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Properties;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($this->propertyInfos as $propertyInfo) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $fieldSynopsisElement = $propertyInfo->getFieldSynopsisElement($doc, $allConstInfos);
                $classSynopsis->appendChild($fieldSynopsisElement);
            }
        }

        $this->appendInheritedMemberSectionToClassSynopsis(
            $doc,
            $classSynopsis,
            $parentsWithInheritedProperties,
            "&Properties;",
            "&InheritedProperties;"
        );

        if (!empty($this->funcInfos)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&Methods;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            $classReference = self::getClassSynopsisReference($this->name);
            $escapedName = addslashes($this->name->__toString());

            if ($this->hasConstructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:constructorsynopsis[@role='$escapedName'])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasMethods()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:methodsynopsis[@role='$escapedName'])"
                );
                $classSynopsis->appendChild($includeElement);
            }

            if ($this->hasDestructor()) {
                $classSynopsis->appendChild(new DOMText("\n    "));
                $includeElement = $this->createIncludeElement(
                    $doc,
                    "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$classReference')/db:refentry/db:refsect1[@role='description']/descendant::db:destructorsynopsis[@role='$escapedName'])"
                );
                $classSynopsis->appendChild($includeElement);
            }
        }

        if (!empty($parentsWithInheritedMethods)) {
            $classSynopsis->appendChild(new DOMText("\n\n    "));
            $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "&InheritedMethods;");
            $classSynopsisInfo->setAttribute("role", "comment");
            $classSynopsis->appendChild($classSynopsisInfo);

            foreach ($parentsWithInheritedMethods as $parent) {
                $parentName = $parent["name"];
                $parentMethodsynopsisTypes = $parent["types"];

                $parentReference = self::getClassSynopsisReference($parentName);
                $escapedParentName = addslashes($parentName->__toString());

                foreach ($parentMethodsynopsisTypes as $parentMethodsynopsisType) {
                    $classSynopsis->appendChild(new DOMText("\n    "));
                    $includeElement = $this->createIncludeElement(
                        $doc,
                        "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:refentry/db:refsect1[@role='description']/descendant::db:{$parentMethodsynopsisType}[@role='$escapedParentName'])"
                    );

                    $classSynopsis->appendChild($includeElement);
                }
            }
        }

        $classSynopsis->appendChild(new DOMText("\n   "));

        return $classSynopsis;
    }

    private static function createOoElement(
        DOMDocument $doc,
        ClassInfo $classInfo,
        ?string $typeOverride,
        bool $withModifiers,
        ?string $modifierOverride,
        int $indentationLevel
    ): ?DOMElement {
        $indentation = str_repeat(" ", $indentationLevel);

        if ($classInfo->type !== "class" && $classInfo->type !== "interface") {
            echo "Class synopsis generation is not implemented for " . $classInfo->type . "\n";
            return null;
        }

        $type = $typeOverride !== null ? $typeOverride : $classInfo->type;

        $ooElement = $doc->createElement("oo$type");
        $ooElement->appendChild(new DOMText("\n$indentation "));
        if ($modifierOverride !== null) {
            $ooElement->appendChild($doc->createElement('modifier', $modifierOverride));
            $ooElement->appendChild(new DOMText("\n$indentation "));
        } elseif ($withModifiers) {
            if ($classInfo->flags & Class_::MODIFIER_FINAL) {
                $ooElement->appendChild($doc->createElement('modifier', 'final'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
            if ($classInfo->flags & Class_::MODIFIER_ABSTRACT) {
                $ooElement->appendChild($doc->createElement('modifier', 'abstract'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
            if ($classInfo->flags & Class_::MODIFIER_READONLY) {
                $ooElement->appendChild($doc->createElement('modifier', 'readonly'));
                $ooElement->appendChild(new DOMText("\n$indentation "));
            }
        }

        $nameElement = $doc->createElement("{$type}name", $classInfo->name->toString());
        $ooElement->appendChild($nameElement);
        $ooElement->appendChild(new DOMText("\n$indentation"));

        return $ooElement;
    }

    public static function getClassSynopsisFilename(Name $name): string {
        return strtolower(str_replace("_", "-", implode('-', $name->getParts())));
    }

    public static function getClassSynopsisReference(Name $name): string {
        return "class." . self::getClassSynopsisFilename($name);
    }

    /**
     * @param array<string, Name> $parentsWithInheritedConstants
     * @param array<string, Name> $parentsWithInheritedProperties
     * @param array<string, array{name: Name, types: int[]}> $parentsWithInheritedMethods
     * @param array<string, ClassInfo> $classMap
     */
    private function collectInheritedMembers(
        array &$parentsWithInheritedConstants,
        array &$parentsWithInheritedProperties,
        array &$parentsWithInheritedMethods,
        bool $hasConstructor,
        array $classMap
    ): void {
        foreach ($this->extends as $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            $parentName = $parent->toString();

            if (!$parentInfo) {
                throw new Exception("Missing parent class $parentName");
            }

            if (!empty($parentInfo->constInfos) && !isset($parentsWithInheritedConstants[$parentName])) {
                $parentsWithInheritedConstants[] = $parent;
            }

            if (!empty($parentInfo->propertyInfos) && !isset($parentsWithInheritedProperties[$parentName])) {
                $parentsWithInheritedProperties[$parentName] = $parent;
            }

            if (!$hasConstructor && $parentInfo->hasNonPrivateConstructor()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "constructorsynopsis";
            }

            if ($parentInfo->hasMethods()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "methodsynopsis";
            }

            if ($parentInfo->hasDestructor()) {
                $parentsWithInheritedMethods[$parentName]["name"] = $parent;
                $parentsWithInheritedMethods[$parentName]["types"][] = "destructorsynopsis";
            }

            $parentInfo->collectInheritedMembers(
                $parentsWithInheritedConstants,
                $parentsWithInheritedProperties,
                $parentsWithInheritedMethods,
                $hasConstructor,
                $classMap
            );
        }

        foreach ($this->implements as $parent) {
            $parentInfo = $classMap[$parent->toString()] ?? null;
            if (!$parentInfo) {
                throw new Exception("Missing parent interface " . $parent->toString());
            }

            if (!empty($parentInfo->constInfos) && !isset($parentsWithInheritedConstants[$parent->toString()])) {
                $parentsWithInheritedConstants[$parent->toString()] = $parent;
            }

            $unusedParentsWithInheritedProperties = [];
            $unusedParentsWithInheritedMethods = [];

            $parentInfo->collectInheritedMembers(
                $parentsWithInheritedConstants,
                $unusedParentsWithInheritedProperties,
                $unusedParentsWithInheritedMethods,
                $hasConstructor,
                $classMap
            );
        }
    }

    /** @param array<string, ClassInfo> $classMap */
    private function isException(array $classMap): bool
    {
        if ($this->name->toString() === "Throwable") {
            return true;
        }

        foreach ($this->extends as $parentName) {
            $parent = $classMap[$parentName->toString()] ?? null;
            if ($parent === null) {
                throw new Exception("Missing parent class " . $parentName->toString());
            }

            if ($parent->isException($classMap)) {
                return true;
            }
        }

        if ($this->type === "class") {
            foreach ($this->implements as $interfaceName) {
                $interface = $classMap[$interfaceName->toString()] ?? null;
                if ($interface === null) {
                    throw new Exception("Missing implemented interface " . $interfaceName->toString());
                }

                if ($interface->isException($classMap)) {
                    return true;
                }
            }
        }

        return false;
    }

    private function hasConstructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isConstructor()) {
                return true;
            }
        }

        return false;
    }

    private function hasNonPrivateConstructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isConstructor() && !($funcInfo->flags & Class_::MODIFIER_PRIVATE)) {
                return true;
            }
        }

        return false;
    }

    private function hasDestructor(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if ($funcInfo->name->isDestructor()) {
                return true;
            }
        }

        return false;
    }

    private function hasMethods(): bool
    {
        foreach ($this->funcInfos as $funcInfo) {
            if (!$funcInfo->name->isConstructor() && !$funcInfo->name->isDestructor()) {
                return true;
            }
        }

        return false;
    }

    private function createIncludeElement(DOMDocument $doc, string $query): DOMElement
    {
        $includeElement = $doc->createElement("xi:include");
        $attr = $doc->createAttribute("xpointer");
        $attr->value = $query;
        $includeElement->appendChild($attr);
        $fallbackElement = $doc->createElement("xi:fallback");
        $includeElement->appendChild(new DOMText("\n     "));
        $includeElement->appendChild($fallbackElement);
        $includeElement->appendChild(new DOMText("\n    "));

        return $includeElement;
    }

    public function __clone()
    {
        foreach ($this->propertyInfos as $key => $propertyInfo) {
            $this->propertyInfos[$key] = clone $propertyInfo;
        }

        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }
    }

    /**
     * @param Name[] $parents
     */
    private function appendInheritedMemberSectionToClassSynopsis(DOMDocument $doc, DOMElement $classSynopsis, array $parents, string $label, string $inheritedLabel): void
    {
        if (empty($parents)) {
            return;
        }

        $classSynopsis->appendChild(new DOMText("\n\n    "));
        $classSynopsisInfo = $doc->createElement("classsynopsisinfo", "$inheritedLabel");
        $classSynopsisInfo->setAttribute("role", "comment");
        $classSynopsis->appendChild($classSynopsisInfo);

        foreach ($parents as $parent) {
            $classSynopsis->appendChild(new DOMText("\n    "));
            $parentReference = self::getClassSynopsisReference($parent);

            $includeElement = $this->createIncludeElement(
                $doc,
                "xmlns(db=http://docbook.org/ns/docbook) xpointer(id('$parentReference')/db:partintro/db:section/db:classsynopsis/db:fieldsynopsis[preceding-sibling::db:classsynopsisinfo[1][@role='comment' and text()='$label']]))"
            );
            $classSynopsis->appendChild($includeElement);
        }
    }
}

class FileInfo {
    /** @var string[] */
    public array $dependencies = [];
    /** @var ConstInfo[] */
    public array $constInfos = [];
    /** @var FuncInfo[] */
    public array $funcInfos = [];
    /** @var ClassInfo[] */
    public array $classInfos = [];
    public bool $generateFunctionEntries = false;
    public string $declarationPrefix = "";
    public ?int $generateLegacyArginfoForPhpVersionId = null;
    public bool $generateClassEntries = false;
    public bool $isUndocumentable = false;

    /**
     * @return iterable<FuncInfo>
     */
    public function getAllFuncInfos(): iterable {
        yield from $this->funcInfos;
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->funcInfos;
        }
    }

    /** @return array<string, ConstInfo> */
    public function getAllConstInfos(): array {
        $result = [];

        foreach ($this->constInfos as $constInfo) {
            $result[$constInfo->name->__toString()] = $constInfo;
        }

        foreach ($this->classInfos as $classInfo) {
            foreach ($classInfo->constInfos as $constInfo) {
                $result[$constInfo->name->__toString()] = $constInfo;
            }
        }

        return $result;
    }

    /**
     * @return iterable<PropertyInfo>
     */
    public function getAllPropertyInfos(): iterable {
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->propertyInfos;
        }
    }

    public function __clone()
    {
        foreach ($this->funcInfos as $key => $funcInfo) {
            $this->funcInfos[$key] = clone $funcInfo;
        }

        foreach ($this->classInfos as $key => $classInfo) {
            $this->classInfos[$key] = clone $classInfo;
        }
    }
}

class DocCommentTag {
    public string $name;
    public ?string $value;

    public function __construct(string $name, ?string $value) {
        $this->name = $name;
        $this->value = $value;
    }

    public function getValue(): string {
        if ($this->value === null) {
            throw new Exception("@$this->name does not have a value");
        }

        return $this->value;
    }

    public function getType(): string {
        $value = $this->getValue();

        $matches = [];

        if ($this->name === "param") {
            preg_match('/^\s*([\w\|\\\\\[\]<>, ]+)\s*(?:[{(]|\$\w+).*$/', $value, $matches);
        } elseif ($this->name === "return" || $this->name === "var") {
            preg_match('/^\s*([\w\|\\\\\[\]<>, ]+)/', $value, $matches);
        }

        if (!isset($matches[1])) {
            throw new Exception("@$this->name doesn't contain a type or has an invalid format \"$value\"");
        }

        return trim($matches[1]);
    }

    public function getVariableName(): string {
        $value = $this->value;
        if ($value === null || strlen($value) === 0) {
            throw new Exception("@$this->name doesn't have any value");
        }

        $matches = [];

        if ($this->name === "param") {
            // Allow for parsing extended types like callable(string):mixed in docblocks
            preg_match('/^\s*(?<type>[\w\|\\\\]+(?<parens>\((?<inparens>(?:(?&parens)|[^(){}[\]]*+))++\)|\{(?&inparens)\}|\[(?&inparens)\])*+(?::(?&type))?)\s*\$(?<name>\w+).*$/', $value, $matches);
        } elseif ($this->name === "prefer-ref") {
            preg_match('/^\s*\$(?<name>\w+).*$/', $value, $matches);
        }

        if (!isset($matches["name"])) {
            throw new Exception("@$this->name doesn't contain a variable name or has an invalid format \"$value\"");
        }

        return $matches["name"];
    }
}

/** @return DocCommentTag[] */
function parseDocComment(DocComment $comment): array {
    $commentText = substr($comment->getText(), 2, -2);
    $tags = [];
    foreach (explode("\n", $commentText) as $commentLine) {
        $regex = '/^\*\s*@([a-z-]+)(?:\s+(.+))?$/';
        if (preg_match($regex, trim($commentLine), $matches)) {
            $tags[] = new DocCommentTag($matches[1], $matches[2] ?? null);
        }
    }

    return $tags;
}

function parseFunctionLike(
    PrettyPrinterAbstract $prettyPrinter,
    FunctionOrMethodName $name,
    int $classFlags,
    int $flags,
    Node\FunctionLike $func,
    ?string $cond,
    bool $isUndocumentable
): FuncInfo {
    try {
        $comment = $func->getDocComment();
        $paramMeta = [];
        $aliasType = null;
        $alias = null;
        $isDeprecated = false;
        $supportsCompileTimeEval = false;
        $verify = true;
        $docReturnType = null;
        $tentativeReturnType = false;
        $docParamTypes = [];
        $refcount = null;

        if ($comment) {
            $tags = parseDocComment($comment);
            foreach ($tags as $tag) {
                switch ($tag->name) {
                    case 'alias':
                    case 'implementation-alias':
                        $aliasType = $tag->name;
                        $aliasParts = explode("::", $tag->getValue());
                        if (count($aliasParts) === 1) {
                            $alias = new FunctionName(new Name($aliasParts[0]));
                        } else {
                            $alias = new MethodName(new Name($aliasParts[0]), $aliasParts[1]);
                        }
                        break;

                    case 'deprecated':
                        $isDeprecated = true;
                        break;

                    case 'no-verify':
                        $verify = false;
                        break;

                    case 'tentative-return-type':
                        $tentativeReturnType = true;
                        break;

                    case 'return':
                        $docReturnType = $tag->getType();
                        break;

                    case 'param':
                        $docParamTypes[$tag->getVariableName()] = $tag->getType();
                        break;

                    case 'refcount':
                        $refcount = $tag->getValue();
                        break;

                    case 'compile-time-eval':
                        $supportsCompileTimeEval = true;
                        break;

                    case 'prefer-ref':
                        $varName = $tag->getVariableName();
                        if (!isset($paramMeta[$varName])) {
                            $paramMeta[$varName] = [];
                        }
                        $paramMeta[$varName][$tag->name] = true;
                        break;

                    case 'undocumentable':
                        $isUndocumentable = true;
                        break;
                }
            }
        }

        $varNameSet = [];
        $args = [];
        $numRequiredArgs = 0;
        $foundVariadic = false;
        foreach ($func->getParams() as $i => $param) {
            $varName = $param->var->name;
            $preferRef = !empty($paramMeta[$varName]['prefer-ref']);
            unset($paramMeta[$varName]);

            if (isset($varNameSet[$varName])) {
                throw new Exception("Duplicate parameter name $varName");
            }
            $varNameSet[$varName] = true;

            if ($preferRef) {
                $sendBy = ArgInfo::SEND_PREFER_REF;
            } else if ($param->byRef) {
                $sendBy = ArgInfo::SEND_BY_REF;
            } else {
                $sendBy = ArgInfo::SEND_BY_VAL;
            }

            if ($foundVariadic) {
                throw new Exception("Only the last parameter can be variadic");
            }

            $type = $param->type ? Type::fromNode($param->type) : null;
            if ($type === null && !isset($docParamTypes[$varName])) {
                throw new Exception("Missing parameter type");
            }

            if ($param->default instanceof Expr\ConstFetch &&
                $param->default->name->toLowerString() === "null" &&
                $type && !$type->isNullable()
            ) {
                $simpleType = $type->tryToSimpleType();
                if ($simpleType === null) {
                    throw new Exception("Parameter $varName has null default, but is not nullable");
                }
            }

            if ($param->default instanceof Expr\ClassConstFetch && $param->default->class->toLowerString() === "self") {
                throw new Exception('The exact class name must be used instead of "self"');
            }

            $foundVariadic = $param->variadic;

            $args[] = new ArgInfo(
                $varName,
                $sendBy,
                $param->variadic,
                $type,
                isset($docParamTypes[$varName]) ? Type::fromString($docParamTypes[$varName]) : null,
                $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null,
                createAttributes($param->attrGroups)
            );
            if (!$param->default && !$param->variadic) {
                $numRequiredArgs = $i + 1;
            }
        }

        foreach (array_keys($paramMeta) as $var) {
            throw new Exception("Found metadata for invalid param $var");
        }

        $returnType = $func->getReturnType();
        if ($returnType === null && $docReturnType === null && !$name->isConstructor() && !$name->isDestructor()) {
            throw new Exception("Missing return type");
        }

        $return = new ReturnInfo(
            $func->returnsByRef(),
            $returnType ? Type::fromNode($returnType) : null,
            $docReturnType ? Type::fromString($docReturnType) : null,
            $tentativeReturnType,
            $refcount
        );

        return new FuncInfo(
            $name,
            $classFlags,
            $flags,
            $aliasType,
            $alias,
            $isDeprecated,
            $supportsCompileTimeEval,
            $verify,
            $args,
            $return,
            $numRequiredArgs,
            $cond,
            $isUndocumentable,
            createAttributes($func->attrGroups)
        );
    } catch (Exception $e) {
        throw new Exception($name . "(): " .$e->getMessage());
    }
}

/**
 * @param array<int, array<int, AttributeGroup> $attributes
 */
function parseConstLike(
    PrettyPrinterAbstract $prettyPrinter,
    ConstOrClassConstName $name,
    Node\Const_ $const,
    int $flags,
    ?Node $type,
    ?DocComment $docComment,
    ?string $cond,
    bool $isUndocumentable,
    ?int $phpVersionIdMinimumCompatibility,
    array $attributes
): ConstInfo {
    $phpDocType = null;
    $deprecated = false;
    $cValue = null;
    $link = null;
    if ($docComment) {
        $tags = parseDocComment($docComment);
        foreach ($tags as $tag) {
            if ($tag->name === 'var') {
                $phpDocType = $tag->getType();
            } elseif ($tag->name === 'deprecated') {
                $deprecated = true;
            } elseif ($tag->name === 'cvalue') {
                $cValue = $tag->value;
            } elseif ($tag->name === 'undocumentable') {
                $isUndocumentable = true;
            } elseif ($tag->name === 'link') {
                $link = $tag->value;
            }
        }
    }

    if ($type === null && $phpDocType === null) {
        throw new Exception("Missing type for constant " . $name->__toString());
    }

    return new ConstInfo(
        $name,
        $flags,
        $const->value,
        $prettyPrinter->prettyPrintExpr($const->value),
        $type ? Type::fromNode($type) : null,
        $phpDocType ? Type::fromString($phpDocType) : null,
        $deprecated,
        $cond,
        $cValue,
        $isUndocumentable,
        $link,
        $phpVersionIdMinimumCompatibility,
        $attributes
    );
}

/**
 * @param array<int, array<int, AttributeGroup> $attributes
 */
function parseProperty(
    Name $class,
    int $flags,
    Stmt\PropertyProperty $property,
    ?Node $type,
    ?DocComment $comment,
    PrettyPrinterAbstract $prettyPrinter,
    ?int $phpVersionIdMinimumCompatibility,
    array $attributes
): PropertyInfo {
    $phpDocType = null;
    $isDocReadonly = false;
    $link = null;

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'var') {
                $phpDocType = $tag->getType();
            } elseif ($tag->name === 'readonly') {
                $isDocReadonly = true;
            } elseif ($tag->name === 'link') {
                $link = $tag->value;
            }
        }
    }

    $propertyType = $type ? Type::fromNode($type) : null;
    if ($propertyType === null && !$phpDocType) {
        throw new Exception("Missing type for property $class::\$$property->name");
    }

    if ($property->default instanceof Expr\ConstFetch &&
        $property->default->name->toLowerString() === "null" &&
        $propertyType && !$propertyType->isNullable()
    ) {
        $simpleType = $propertyType->tryToSimpleType();
        if ($simpleType === null) {
            throw new Exception(
                "Property $class::\$$property->name has null default, but is not nullable");
        }
    }

    return new PropertyInfo(
        new PropertyName($class, $property->name->__toString()),
        $flags,
        $propertyType,
        $phpDocType ? Type::fromString($phpDocType) : null,
        $property->default,
        $property->default ? $prettyPrinter->prettyPrintExpr($property->default) : null,
        $isDocReadonly,
        $link,
        $phpVersionIdMinimumCompatibility,
        $attributes
    );
}

/**
 * @param ConstInfo[] $consts
 * @param PropertyInfo[] $properties
 * @param FuncInfo[] $methods
 * @param EnumCaseInfo[] $enumCases
 */
function parseClass(
    Name $name,
    Stmt\ClassLike $class,
    array $consts,
    array $properties,
    array $methods,
    array $enumCases,
    ?string $cond,
    ?int $minimumPhpVersionIdCompatibility,
    bool $isUndocumentable
): ClassInfo {
    $flags = $class instanceof Class_ ? $class->flags : 0;
    $comment = $class->getDocComment();
    $alias = null;
    $isDeprecated = false;
    $isStrictProperties = false;
    $isNotSerializable = false;
    $allowsDynamicProperties = false;
    $attributes = [];

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'alias') {
                $alias = $tag->getValue();
            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
            } else if ($tag->name === 'strict-properties') {
                $isStrictProperties = true;
            } else if ($tag->name === 'not-serializable') {
                $isNotSerializable = true;
            } else if ($tag->name === 'undocumentable') {
                $isUndocumentable = true;
            }
        }
    }

    $attributes = createAttributes($class->attrGroups);
    foreach ($attributes as $attribute) {
        switch ($attribute->class) {
            case 'AllowDynamicProperties':
                $allowsDynamicProperties = true;
                break 2;
        }
    }

    if ($isStrictProperties && $allowsDynamicProperties) {
        throw new Exception("A class may not have '@strict-properties' and '#[\\AllowDynamicProperties]' at the same time.");
    }

    $extends = [];
    $implements = [];

    if ($class instanceof Class_) {
        $classKind = "class";
        if ($class->extends) {
            $extends[] = $class->extends;
        }
        $implements = $class->implements;
    } elseif ($class instanceof Interface_) {
        $classKind = "interface";
        $extends = $class->extends;
    } else if ($class instanceof Trait_) {
        $classKind = "trait";
    } else if ($class instanceof Enum_) {
        $classKind = "enum";
        $implements = $class->implements;
    } else {
        throw new Exception("Unknown class kind " . get_class($class));
    }

    if ($isUndocumentable) {
        foreach ($methods as $method) {
            $method->isUndocumentable = true;
        }
    }

    return new ClassInfo(
        $name,
        $flags,
        $classKind,
        $alias,
        $class instanceof Enum_ && $class->scalarType !== null
            ? SimpleType::fromNode($class->scalarType) : null,
        $isDeprecated,
        $isStrictProperties,
        $attributes,
        $isNotSerializable,
        $extends,
        $implements,
        $consts,
        $properties,
        $methods,
        $enumCases,
        $cond,
        $minimumPhpVersionIdCompatibility,
        $isUndocumentable
    );
}

/**
 * @param array<int, array<int, AttributeGroup>> $attributeGroups
 * @return Attribute[]
 */
function createAttributes(array $attributeGroups): array {
    $attributes = [];

    foreach ($attributeGroups as $attrGroup) {
        foreach ($attrGroup->attrs as $attr) {
            $attributes[] = new AttributeInfo($attr->name->toString(), $attr->args);
        }
    }

    return $attributes;
}

function handlePreprocessorConditions(array &$conds, Stmt $stmt): ?string {
    foreach ($stmt->getComments() as $comment) {
        $text = trim($comment->getText());
        if (preg_match('/^#\s*if\s+(.+)$/', $text, $matches)) {
            $conds[] = $matches[1];
        } else if (preg_match('/^#\s*ifdef\s+(.+)$/', $text, $matches)) {
            $conds[] = "defined($matches[1])";
        } else if (preg_match('/^#\s*ifndef\s+(.+)$/', $text, $matches)) {
            $conds[] = "!defined($matches[1])";
        } else if (preg_match('/^#\s*else$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered else without corresponding #if");
            }
            $cond = array_pop($conds);
            $conds[] = "!($cond)";
        } else if (preg_match('/^#\s*endif$/', $text)) {
            if (empty($conds)) {
                throw new Exception("Encountered #endif without corresponding #if");
            }
            array_pop($conds);
        } else if ($text[0] === '#') {
            throw new Exception("Unrecognized preprocessor directive \"$text\"");
        }
    }

    return empty($conds) ? null : implode(' && ', $conds);
}

function getFileDocComment(array $stmts): ?DocComment {
    if (empty($stmts)) {
        return null;
    }

    $comments = $stmts[0]->getComments();
    if (empty($comments)) {
        return null;
    }

    if ($comments[0] instanceof DocComment) {
        return $comments[0];
    }

    return null;
}

function handleStatements(FileInfo $fileInfo, array $stmts, PrettyPrinterAbstract $prettyPrinter) {
    $conds = [];
    foreach ($stmts as $stmt) {
        $cond = handlePreprocessorConditions($conds, $stmt);

        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Namespace_) {
            handleStatements($fileInfo, $stmt->stmts, $prettyPrinter);
            continue;
        }

        if ($stmt instanceof Stmt\Const_) {
            foreach ($stmt->consts as $const) {
                $fileInfo->constInfos[] = parseConstLike(
                    $prettyPrinter,
                    new ConstName($const->namespacedName, $const->name->toString()),
                    $const,
                    0,
                    null,
                    $stmt->getDocComment(),
                    $cond,
                    $fileInfo->isUndocumentable,
                    $fileInfo->generateLegacyArginfoForPhpVersionId,
                    []
                );
            }
            continue;
        }

        if ($stmt instanceof Stmt\Function_) {
            $fileInfo->funcInfos[] = parseFunctionLike(
                $prettyPrinter,
                new FunctionName($stmt->namespacedName),
                0,
                0,
                $stmt,
                $cond,
                $fileInfo->isUndocumentable
            );
            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $className = $stmt->namespacedName;
            $constInfos = [];
            $propertyInfos = [];
            $methodInfos = [];
            $enumCaseInfos = [];
            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                $classFlags = $stmt instanceof Class_ ? $stmt->flags : 0;
                $abstractFlag = $stmt instanceof Stmt\Interface_ ? Class_::MODIFIER_ABSTRACT : 0;

                if ($classStmt instanceof Stmt\ClassConst) {
                    foreach ($classStmt->consts as $const) {
                        $constInfos[] = parseConstLike(
                            $prettyPrinter,
                            new ClassConstName($className, $const->name->toString()),
                            $const,
                            $classStmt->flags,
                            $classStmt->type,
                            $classStmt->getDocComment(),
                            $cond,
                            $fileInfo->isUndocumentable,
                            $fileInfo->generateLegacyArginfoForPhpVersionId,
                            createAttributes($classStmt->attrGroups)
                        );
                    }
                } else if ($classStmt instanceof Stmt\Property) {
                    if (!($classStmt->flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                        throw new Exception("Visibility modifier is required");
                    }
                    foreach ($classStmt->props as $property) {
                        $propertyInfos[] = parseProperty(
                            $className,
                            $classStmt->flags,
                            $property,
                            $classStmt->type,
                            $classStmt->getDocComment(),
                            $prettyPrinter,
                            $fileInfo->generateLegacyArginfoForPhpVersionId,
                            createAttributes($classStmt->attrGroups)
                        );
                    }
                } else if ($classStmt instanceof Stmt\ClassMethod) {
                    if (!($classStmt->flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                        throw new Exception("Visibility modifier is required");
                    }
                    $methodInfos[] = parseFunctionLike(
                        $prettyPrinter,
                        new MethodName($className, $classStmt->name->toString()),
                        $classFlags,
                        $classStmt->flags | $abstractFlag,
                        $classStmt,
                        $cond,
                        $fileInfo->isUndocumentable
                    );
                } else if ($classStmt instanceof Stmt\EnumCase) {
                    $enumCaseInfos[] = new EnumCaseInfo(
                        $classStmt->name->toString(), $classStmt->expr);
                } else {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }
            }

            $fileInfo->classInfos[] = parseClass(
                $className, $stmt, $constInfos, $propertyInfos, $methodInfos, $enumCaseInfos, $cond, $fileInfo->generateLegacyArginfoForPhpVersionId, $fileInfo->isUndocumentable
            );
            continue;
        }

        if ($stmt instanceof Stmt\Expression) {
            $expr = $stmt->expr;
            if ($expr instanceof Expr\Include_) {
                $fileInfo->dependencies[] = (string)EvaluatedValue::createFromExpression($expr->expr, null, null, [])->value;
                continue;
            }
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }
    if (!empty($conds)) {
        throw new Exception("Unterminated preprocessor conditions");
    }
}

function parseStubFile(string $code): FileInfo {
    $lexer = new PhpParser\Lexer\Emulative();
    $parser = new PhpParser\Parser\Php7($lexer);
    $nodeTraverser = new PhpParser\NodeTraverser;
    $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);
    $prettyPrinter = new class extends Standard {
        protected function pName_FullyQualified(Name\FullyQualified $node): string {
            return implode('\\', $node->getParts());
        }
    };

    $stmts = $parser->parse($code);
    $nodeTraverser->traverse($stmts);

    $fileInfo = new FileInfo;
    $fileDocComment = getFileDocComment($stmts);
    if ($fileDocComment) {
        $fileTags = parseDocComment($fileDocComment);
        foreach ($fileTags as $tag) {
            if ($tag->name === 'generate-function-entries') {
                $fileInfo->generateFunctionEntries = true;
                $fileInfo->declarationPrefix = $tag->value ? $tag->value . " " : "";
            } else if ($tag->name === 'generate-legacy-arginfo') {
                if ($tag->value && !in_array((int) $tag->value, ALL_PHP_VERSION_IDS, true)) {
                    throw new Exception(
                        "Legacy PHP version must be one of: \"" . PHP_70_VERSION_ID . "\" (PHP 7.0), \"" . PHP_80_VERSION_ID . "\" (PHP 8.0), " .
                        "\"" . PHP_81_VERSION_ID . "\" (PHP 8.1), \"" . PHP_82_VERSION_ID . "\" (PHP 8.2), \"" . PHP_83_VERSION_ID . "\" (PHP 8.3), " .
                        "\"" . $tag->value . "\" provided"
                    );
                }

                $fileInfo->generateLegacyArginfoForPhpVersionId = $tag->value ? (int) $tag->value : PHP_70_VERSION_ID;
            } else if ($tag->name === 'generate-class-entries') {
                $fileInfo->generateClassEntries = true;
                $fileInfo->declarationPrefix = $tag->value ? $tag->value . " " : "";
            } else if ($tag->name === 'undocumentable') {
                $fileInfo->isUndocumentable = true;
            }
        }
    }

    // Generating class entries require generating function/method entries
    if ($fileInfo->generateClassEntries && !$fileInfo->generateFunctionEntries) {
        $fileInfo->generateFunctionEntries = true;
    }

    handleStatements($fileInfo, $stmts, $prettyPrinter);
    return $fileInfo;
}

function funcInfoToCode(FileInfo $fileInfo, FuncInfo $funcInfo): string {
    $code = '';
    $returnType = $funcInfo->return->type;
    $isTentativeReturnType = $funcInfo->return->tentativeReturnType;
    $php81MinimumCompatibility = $fileInfo->generateLegacyArginfoForPhpVersionId === null || $fileInfo->generateLegacyArginfoForPhpVersionId >= PHP_81_VERSION_ID;

    if ($returnType !== null) {
        if ($isTentativeReturnType && !$php81MinimumCompatibility) {
            $code .= "#if (PHP_VERSION_ID >= " . PHP_81_VERSION_ID . ")\n";
        }
        if (null !== $simpleReturnType = $returnType->tryToSimpleType()) {
            if ($simpleReturnType->isBuiltin) {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %d)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toTypeCode(), $returnType->isNullable()
                );
            } else {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %d)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_INFO_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toEscapedName(), $returnType->isNullable()
                );
            }
        } else {
            $arginfoType = $returnType->toArginfoType();
            if ($arginfoType->hasClassType()) {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s, %s)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_OBJ_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toClassTypeString(), $arginfoType->toTypeMask()
                );
            } else {
                $code .= sprintf(
                    "%s(%s, %d, %d, %s)\n",
                    $isTentativeReturnType ? "ZEND_BEGIN_ARG_WITH_TENTATIVE_RETURN_TYPE_MASK_EX" : "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toTypeMask()
                );
            }
        }
        if ($isTentativeReturnType && !$php81MinimumCompatibility) {
            $code .= sprintf(
                "#else\nZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n#endif\n",
                $funcInfo->getArgInfoName(), $funcInfo->return->byRef, $funcInfo->numRequiredArgs
            );
        }
    } else {
        $code .= sprintf(
            "ZEND_BEGIN_ARG_INFO_EX(%s, 0, %d, %d)\n",
            $funcInfo->getArgInfoName(), $funcInfo->return->byRef, $funcInfo->numRequiredArgs
        );
    }

    foreach ($funcInfo->args as $argInfo) {
        $argKind = $argInfo->isVariadic ? "ARG_VARIADIC" : "ARG";
        $argDefaultKind = $argInfo->hasProperDefaultValue() ? "_WITH_DEFAULT_VALUE" : "";
        $argType = $argInfo->type;
        if ($argType !== null) {
            if (null !== $simpleArgType = $argType->tryToSimpleType()) {
                if ($simpleArgType->isBuiltin) {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toTypeCode(), $argType->isNullable(),
                        $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_INFO%s(%s, %s, %s, %d%s)\n",
                        $argKind,$argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                        $simpleArgType->toEscapedName(), $argType->isNullable(),
                        $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
                    );
                }
            } else {
                $arginfoType = $argType->toArginfoType();
                if ($arginfoType->hasClassType()) {
                    $code .= sprintf(
                        "\tZEND_%s_OBJ_TYPE_MASK(%s, %s, %s, %s%s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $arginfoType->toClassTypeString(), $arginfoType->toTypeMask(),
                        !$argInfo->isVariadic ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
                    );
                } else {
                    $code .= sprintf(
                        "\tZEND_%s_TYPE_MASK(%s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $arginfoType->toTypeMask(),
                        $argInfo->getDefaultValueAsArginfoString()
                    );
                }
            }
        } else {
            $code .= sprintf(
                "\tZEND_%s_INFO%s(%s, %s%s)\n",
                $argKind, $argDefaultKind, $argInfo->getSendByString(), $argInfo->name,
                $argInfo->hasProperDefaultValue() ? ", " . $argInfo->getDefaultValueAsArginfoString() : ""
            );
        }
    }

    $code .= "ZEND_END_ARG_INFO()";
    return $code . "\n";
}

/** @param FuncInfo[] $generatedFuncInfos */
function findEquivalentFuncInfo(array $generatedFuncInfos, FuncInfo $funcInfo): ?FuncInfo {
    foreach ($generatedFuncInfos as $generatedFuncInfo) {
        if ($generatedFuncInfo->equalsApartFromNameAndRefcount($funcInfo)) {
            return $generatedFuncInfo;
        }
    }
    return null;
}

/**
 * @template T
 * @param iterable<T> $infos
 * @param Closure(T): string|null $codeGenerator
 * @param ?string $parentCond
 */
function generateCodeWithConditions(
    iterable $infos, string $separator, Closure $codeGenerator, ?string $parentCond = null): string {
    $code = "";
    foreach ($infos as $info) {
        $infoCode = $codeGenerator($info);
        if ($infoCode === null) {
            continue;
        }

        $code .= $separator;
        if ($info->cond && $info->cond !== $parentCond) {
            $code .= "#if {$info->cond}\n";
            $code .= $infoCode;
            $code .= "#endif\n";
        } else {
            $code .= $infoCode;
        }
    }

    return $code;
}

/**
 * @param array<string, ConstInfo> $allConstInfos
 */
function generateArgInfoCode(
    string $stubFilenameWithoutExtension,
    FileInfo $fileInfo,
    array $allConstInfos,
    string $stubHash
): string {
    $code = "/* This is a generated file, edit the .stub.php file instead.\n"
          . " * Stub hash: $stubHash */\n";

    $generatedFuncInfos = [];
    $code .= generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        static function (FuncInfo $funcInfo) use (&$generatedFuncInfos, $fileInfo) {
            /* If there already is an equivalent arginfo structure, only emit a #define */
            if ($generatedFuncInfo = findEquivalentFuncInfo($generatedFuncInfos, $funcInfo)) {
                $code = sprintf(
                    "#define %s %s\n",
                    $funcInfo->getArgInfoName(), $generatedFuncInfo->getArgInfoName()
                );
            } else {
                $code = funcInfoToCode($fileInfo, $funcInfo);
            }

            $generatedFuncInfos[] = $funcInfo;
            return $code;
        }
    );

    if ($fileInfo->generateFunctionEntries) {
        $code .= "\n\n";

        $generatedFunctionDeclarations = [];
        $code .= generateCodeWithConditions(
            $fileInfo->getAllFuncInfos(), "",
            static function (FuncInfo $funcInfo) use ($fileInfo, &$generatedFunctionDeclarations) {
                $key = $funcInfo->getDeclarationKey();
                if (isset($generatedFunctionDeclarations[$key])) {
                    return null;
                }

                $generatedFunctionDeclarations[$key] = true;
                return $fileInfo->declarationPrefix . $funcInfo->getDeclaration();
            }
        );

        if (!empty($fileInfo->funcInfos)) {
            $code .= generateFunctionEntries(null, $fileInfo->funcInfos);
        }

        foreach ($fileInfo->classInfos as $classInfo) {
            $code .= generateFunctionEntries($classInfo->name, $classInfo->funcInfos, $classInfo->cond);
        }
    }

    $php80MinimumCompatibility = $fileInfo->generateLegacyArginfoForPhpVersionId === null || $fileInfo->generateLegacyArginfoForPhpVersionId >= PHP_80_VERSION_ID;

    if ($fileInfo->generateClassEntries) {
        if ($attributeInitializationCode = generateFunctionAttributeInitialization($fileInfo->funcInfos, $allConstInfos, $fileInfo->generateLegacyArginfoForPhpVersionId, null)) {
            if (!$php80MinimumCompatibility) {
                $attributeInitializationCode = "\n#if (PHP_VERSION_ID >= " . PHP_80_VERSION_ID . ")" . $attributeInitializationCode . "#endif\n";
            }
        }

        if ($attributeInitializationCode !== "" || !empty($fileInfo->constInfos)) {
            $code .= "\nstatic void register_{$stubFilenameWithoutExtension}_symbols(int module_number)\n";
            $code .= "{\n";

            foreach ($fileInfo->constInfos as $constInfo) {
                $code .= $constInfo->getDeclaration($allConstInfos);
            }

            if (!empty($attributeInitializationCode !== "" && $fileInfo->constInfos)) {
                $code .= "\n";
            }

            $code .= $attributeInitializationCode;
            $code .= "}\n";
        }

        $code .= generateClassEntryCode($fileInfo, $allConstInfos);
    }

    return $code;
}

/** @param array<string, ConstInfo> $allConstInfos */
function generateClassEntryCode(FileInfo $fileInfo, array $allConstInfos): string {
    $code = "";

    foreach ($fileInfo->classInfos as $class) {
        $code .= "\n" . $class->getRegistration($allConstInfos);
    }

    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateFunctionEntries(?Name $className, array $funcInfos, ?string $cond = null): string {
    $code = "\n\n";

    if ($cond) {
        $code .= "#if {$cond}\n";
    }

    $functionEntryName = "ext_functions";
    if ($className) {
        $underscoreName = implode("_", $className->getParts());
        $functionEntryName = "class_{$underscoreName}_methods";
    }

    $code .= "static const zend_function_entry {$functionEntryName}[] = {\n";
    $code .= generateCodeWithConditions($funcInfos, "", static function (FuncInfo $funcInfo) {
        return $funcInfo->getFunctionEntry();
    }, $cond);
    $code .= "\tZEND_FE_END\n";
    $code .= "};\n";

    if ($cond) {
        $code .= "#endif\n";
    }

    return $code;
}

/** @param iterable<FuncInfo> $funcInfos */
function generateFunctionAttributeInitialization(iterable $funcInfos, array $allConstInfos, ?int $phpVersionIdMinimumCompatibility, ?string $parentCond = null): string {
    return generateCodeWithConditions(
        $funcInfos,
        "",
        static function (FuncInfo $funcInfo) use ($allConstInfos, $phpVersionIdMinimumCompatibility) {
            $code = null;

            if ($funcInfo->name instanceof MethodName) {
                $functionTable = "&class_entry->function_table";
            } else {
                $functionTable = "CG(function_table)";
            }

            foreach ($funcInfo->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_function_attribute(zend_hash_str_find_ptr($functionTable, \"" . $funcInfo->name->getNameForAttributes() . "\", sizeof(\"" . $funcInfo->name->getNameForAttributes() . "\") - 1)",
                    "func_" . $funcInfo->name->getNameForAttributes() . "_$key",
                    $allConstInfos,
                    $phpVersionIdMinimumCompatibility
                );
            }

            foreach ($funcInfo->args as $index => $arg) {
                foreach ($arg->attributes as $key => $attribute) {
                    $code .= $attribute->generateCode(
                        "zend_add_parameter_attribute(zend_hash_str_find_ptr($functionTable, \"" . $funcInfo->name->getNameForAttributes() . "\", sizeof(\"" . $funcInfo->name->getNameForAttributes() . "\") - 1), $index",
                        "func_{$funcInfo->name->getNameForAttributes()}_arg{$index}_$key",
                        $allConstInfos,
                        $phpVersionIdMinimumCompatibility
                    );
                }
            }

            return $code;
        },
        $parentCond
    );
}

/**
 * @param iterable<ConstInfo> $constInfos
 * @param array<string, ConstInfo> $allConstInfos
 */
function generateConstantAttributeInitialization(
    iterable $constInfos,
    array $allConstInfos,
    ?int $phpVersionIdMinimumCompatibility,
    ?string $parentCond = null
): string {
    return generateCodeWithConditions(
        $constInfos,
        "",
        static function (ConstInfo $constInfo) use ($allConstInfos, $phpVersionIdMinimumCompatibility) {
            $code = null;

            foreach ($constInfo->attributes as $key => $attribute) {
                $code .= $attribute->generateCode(
                    "zend_add_class_constant_attribute(class_entry, const_" . $constInfo->name->getDeclarationName(),
                    "const_" . $constInfo->name->getDeclarationName() . "_$key",
                    $allConstInfos,
                    $phpVersionIdMinimumCompatibility
                );
            }

            return $code;
        },
        $parentCond
    );
}

/**
 * @param iterable<PropertyInfo> $propertyInfos
 * @param array<string, ConstInfo> $allConstInfos
 */
function generatePropertyAttributeInitialization(
    iterable $propertyInfos,
    array $allConstInfos,
    ?int $phpVersionIdMinimumCompatibility
): string {
    $code = "";
    foreach ($propertyInfos as $propertyInfo) {
        foreach ($propertyInfo->attributes as $key => $attribute) {
            $code .= $attribute->generateCode(
                "zend_add_property_attribute(class_entry, property_" . $propertyInfo->name->getDeclarationName(),
                "property_" . $propertyInfo->name->getDeclarationName() . "_" . $key,
                $allConstInfos,
                $phpVersionIdMinimumCompatibility
            );
        }
    }

    return $code;
}

/** @param array<string, FuncInfo> $funcMap */
function generateOptimizerInfo(array $funcMap): string {

    $code = "/* This is a generated file, edit the .stub.php files instead. */\n\n";

    $code .= "static const func_info_t func_infos[] = {\n";

    $code .= generateCodeWithConditions($funcMap, "", static function (FuncInfo $funcInfo) {
        return $funcInfo->getOptimizerInfo();
    });

    $code .= "};\n";

    return $code;
}

/**
 * @param array<int, string[]> $flagsByPhpVersions
 * @return string[]
 */
function generateVersionDependentFlagCode(string $codeTemplate, array $flagsByPhpVersions, ?int $phpVersionIdMinimumCompatibility): array
{
    $phpVersions = ALL_PHP_VERSION_IDS;
    sort($phpVersions);
    $currentPhpVersion = end($phpVersions);

    // No version compatibility is needed
    if ($phpVersionIdMinimumCompatibility === null) {
        if (empty($flagsByPhpVersions[$currentPhpVersion])) {
            return [];
        }

        return [sprintf($codeTemplate, implode("|", $flagsByPhpVersions[$currentPhpVersion]))];
    }

    // Remove flags which depend on a PHP version below the minimally supported one
    ksort($flagsByPhpVersions);
    $index = array_search($phpVersionIdMinimumCompatibility, array_keys($flagsByPhpVersions));
    if ($index === false) {
        throw new Exception("Missing version dependent flags for PHP version ID \"$phpVersionIdMinimumCompatibility\"");
    }
    $flagsByPhpVersions = array_slice($flagsByPhpVersions, $index, null, true);

    // Remove empty version-specific flags
    $flagsByPhpVersions = array_filter(
        $flagsByPhpVersions,
        static function (array $value): bool {
            return !empty($value);
    });

    // There are no version-specific flags
    if (empty($flagsByPhpVersions)) {
        return [];
    }

    // Remove version-specific flags which don't differ from the previous one
    $previousVersionId = null;
    foreach ($flagsByPhpVersions as $versionId => $versionFlags) {
        if ($previousVersionId !== null && $flagsByPhpVersions[$previousVersionId] === $versionFlags) {
            unset($flagsByPhpVersions[$versionId]);
        } else {
            $previousVersionId = $versionId;
        }
    }

    $flagCount = count($flagsByPhpVersions);

    // Do not add a condition unnecessarily when the only version is the same as the minimally supported one
    if ($flagCount === 1) {
        reset($flagsByPhpVersions);
        $firstVersion = key($flagsByPhpVersions);
        if ($firstVersion === $phpVersionIdMinimumCompatibility) {
            return [sprintf($codeTemplate, implode("|", reset($flagsByPhpVersions)))];
        }
    }

    // Add the necessary conditions around the code using the version-specific flags
    $result = [];
    $i = 0;
    foreach (array_reverse($flagsByPhpVersions, true) as $version => $versionFlags) {
        $code = "";

        $if = $i === 0 ? "#if" : "#elif";
        $endif = $i === $flagCount - 1 ? "#endif\n" : "";

        $code .= "$if (PHP_VERSION_ID >= $version)\n";

        $code .= sprintf($codeTemplate, implode("|", $versionFlags));
        $code .= $endif;

        $result[] = $code;
        $i++;
    }

    return $result;
}

/**
 * @param array<string, ConstInfo> $constMap
 * @param array<string, ConstInfo> $undocumentedConstMap
 * @return array<string, string|null>
 */
function replacePredefinedConstants(string $targetDirectory, array $constMap, array &$undocumentedConstMap): array {
    /** @var array<string, string> $documentedConstMap */
    $documentedConstMap = [];
    /** @var array<string, string> $predefinedConstants */
    $predefinedConstants = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/(?:[\w\.]*constants[\w\.]*|tokens).xml$/i', basename($pathName))) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if (stripos($xml, "<appendix") === false && stripos($xml, "<sect2") === false && stripos($xml, "<chapter") === false) {
            continue;
        }

        $replacedXml = getReplacedSynopsisXml($xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $updated = false;

        foreach ($doc->getElementsByTagName("varlistentry") as $entry) {
            if (!$entry instanceof DOMElement) {
                continue;
            }

            foreach ($entry->getElementsByTagName("term") as $manualTermElement) {
                $manualConstantElement = $manualTermElement->getElementsByTagName("constant")->item(0);
                if (!$manualConstantElement instanceof DOMElement) {
                    continue;
                }

                $manualConstantName = $manualConstantElement->textContent;

                $stubConstant = $constMap[$manualConstantName] ?? null;
                if ($stubConstant === null) {
                    continue;
                }

                $documentedConstMap[$manualConstantName] = $manualConstantName;

                if ($entry->firstChild instanceof DOMText) {
                    $indentationLevel = strlen(str_replace("\n", "", $entry->firstChild->textContent));
                } else {
                    $indentationLevel = 3;
                }
                $newTermElement = $stubConstant->getPredefinedConstantTerm($doc, $indentationLevel);

                if ($manualTermElement->textContent === $newTermElement->textContent) {
                    continue;
                }

                $manualTermElement->parentNode->replaceChild($newTermElement, $manualTermElement);
                $updated = true;
            }
        }

        foreach ($doc->getElementsByTagName("row") as $row) {
            if (!$row instanceof DOMElement) {
                continue;
            }

            $entry = $row->getElementsByTagName("entry")->item(0);
            if (!$entry instanceof DOMElement) {
                continue;
            }

            foreach ($entry->getElementsByTagName("constant") as $manualConstantElement) {
                if (!$manualConstantElement instanceof DOMElement) {
                    continue;
                }

                $manualConstantName = $manualConstantElement->textContent;

                $stubConstant = $constMap[$manualConstantName] ?? null;
                if ($stubConstant === null) {
                    continue;
                }

                $documentedConstMap[$manualConstantName] = $manualConstantName;

                if ($row->firstChild instanceof DOMText) {
                    $indentationLevel = strlen(str_replace("\n", "", $row->firstChild->textContent));
                } else {
                    $indentationLevel = 3;
                }
                $newEntryElement = $stubConstant->getPredefinedConstantEntry($doc, $indentationLevel);

                if ($entry->textContent === $newEntryElement->textContent) {
                    continue;
                }

                $entry->parentNode->replaceChild($newEntryElement, $entry);
                $updated = true;
            }
        }

        if ($updated) {
            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    '/<appendix\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<appendix\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<sect2\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<sect2\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<chapter\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<chapter\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                ],
                [
                    "&$1",
                    "<appendix xml:id=\"$2\" xmlns=\"$1\">",
                    "<appendix xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                    "<sect2 xml:id=\"$2\" xmlns=\"$1\">",
                    "<sect2 xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                    "<chapter xml:id=\"$2\" xmlns=\"$1\">",
                    "<chapter xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                ],
                $replacedXml
            );

            $predefinedConstants[$pathName] = $replacedXml;
        }
    }

    $undocumentedConstMap = array_diff_key($constMap, $documentedConstMap);

    return $predefinedConstants;
}

/**
 * @param array<string, ClassInfo> $classMap
 * @param array<string, ConstInfo> $allConstInfos
 * @return array<string, string>
 */
function generateClassSynopses(array $classMap, array $allConstInfos): array {
    $result = [];

    foreach ($classMap as $classInfo) {
        $classSynopsis = $classInfo->getClassSynopsisDocument($classMap, $allConstInfos);
        if ($classSynopsis !== null) {
            $result[ClassInfo::getClassSynopsisFilename($classInfo->name) . ".xml"] = $classSynopsis;
        }
    }

    return $result;
}

/**
 * @param array<string, ClassInfo> $classMap
 * @param array<string, ConstInfo> $allConstInfos
 * @param array<string, ClassInfo> $undocumentedClassMap
 * @return array<string, string>
 */
function replaceClassSynopses(
    string $targetDirectory,
    array $classMap,
    array $allConstInfos,
    array &$undocumentedClassMap
): array {
    /** @var array<string, string> $documentedClassMap */
    $documentedClassMap = [];
    /** @var array<string, string> $classSynopses */
    $classSynopses = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/\.xml$/i', $pathName)) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if (stripos($xml, "<classsynopsis") === false) {
            continue;
        }

        $replacedXml = getReplacedSynopsisXml($xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $classSynopsisElements = [];
        foreach ($doc->getElementsByTagName("classsynopsis") as $element) {
            $classSynopsisElements[] = $element;
        }

        foreach ($classSynopsisElements as $classSynopsis) {
            if (!$classSynopsis instanceof DOMElement) {
                continue;
            }

            $child = $classSynopsis->firstElementChild;
            if ($child === null) {
                continue;
            }
            $child = $child->lastElementChild;
            if ($child === null) {
                continue;
            }
            $className = $child->textContent;
            if (!isset($classMap[$className])) {
                continue;
            }

            $documentedClassMap[$className] = $className;

            $classInfo = $classMap[$className];

            $newClassSynopsis = $classInfo->getClassSynopsisElement($doc, $classMap, $allConstInfos);
            if ($newClassSynopsis === null) {
                continue;
            }

            // Check if there is any change - short circuit if there is not any.

            if (replaceAndCompareXmls($doc, $classSynopsis, $newClassSynopsis)) {
                continue;
            }

            // Return the updated XML

            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    '/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([^"]+)"\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([^"]+)"\s+xmlns="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([^"]+)"\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<phpdoc:(classref|exceptionref)\s+xmlns:phpdoc=\"([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<phpdoc:(classref|exceptionref)\s+xmlns=\"([^"]+)\"\s+xmlns:xlink="([^"]+)"\s+xmlns:xi="([^"]+)"\s+xmlns:phpdoc="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                ],
                [
                    "&$1",
                    "<phpdoc:$1 xml:id=\"$4\" xmlns:phpdoc=\"$2\" xmlns=\"$3\">",
                    "<phpdoc:$1 xml:id=\"$5\" xmlns:phpdoc=\"$2\" xmlns=\"$3\" xmlns:xi=\"$4\">",
                    "<phpdoc:$1 xml:id=\"$6\" xmlns:phpdoc=\"$2\" xmlns=\"$3\" xmlns:xlink=\"$4\" xmlns:xi=\"$5\">",
                    "<phpdoc:$1 xml:id=\"$6\" xmlns:phpdoc=\"$2\" xmlns=\"$5\" xmlns:xlink=\"$3\" xmlns:xi=\"$4\">",
                    "<phpdoc:$1 xml:id=\"$6\" xmlns:phpdoc=\"$5\" xmlns=\"$2\" xmlns:xlink=\"$3\" xmlns:xi=\"$4\">",
                ],
                $replacedXml
            );

            $classSynopses[$pathName] = $replacedXml;
        }
    }

    $undocumentedClassMap = array_diff_key($classMap, $documentedClassMap);

    return $classSynopses;
}

function getReplacedSynopsisXml(string $xml): string
{
    return preg_replace(
        [
            "/&([A-Za-z0-9._{}%-]+?;)/",
            "/<(\/)*xi:([A-Za-z]+?)/"
        ],
        [
            "REPLACED-ENTITY-$1",
            "<$1XI$2",
        ],
        $xml
    );
}

/**
 * @param array<string, FuncInfo> $funcMap
 * @param array<string, FuncInfo> $aliasMap
 * @return array<string, string>
 */
function generateMethodSynopses(array $funcMap, array $aliasMap): array {
    $result = [];

    foreach ($funcMap as $funcInfo) {
        $methodSynopsis = $funcInfo->getMethodSynopsisDocument($funcMap, $aliasMap);
        if ($methodSynopsis !== null) {
            $result[$funcInfo->name->getMethodSynopsisFilename() . ".xml"] = $methodSynopsis;
        }
    }

    return $result;
}

/**
 * @param array<string, FuncInfo> $funcMap
 * @param array<string, FuncInfo> $aliasMap
 * @param array<int, string> $methodSynopsisWarnings
 * @param array<string, FuncInfo> $undocumentedFuncMap
 * @return array<string, string>
 */
function replaceMethodSynopses(
    string $targetDirectory,
    array $funcMap,
    array $aliasMap,
    bool $isVerifyManual,
    array &$methodSynopsisWarnings,
    array &$undocumentedFuncMap
): array {
    /** @var array<string, string> $documentedFuncMap */
    $documentedFuncMap = [];
    /** @var array<string, string> $methodSynopses */
    $methodSynopses = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($targetDirectory),
        RecursiveIteratorIterator::LEAVES_ONLY
    );

    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (!preg_match('/\.xml$/i', $pathName)) {
            continue;
        }

        $xml = file_get_contents($pathName);
        if ($xml === false) {
            continue;
        }

        if ($isVerifyManual) {
            $matches = [];
            preg_match("/<refname>\s*([\w:]+)\s*<\/refname>\s*<refpurpose>\s*&Alias;\s*<(?:function|methodname)>\s*([\w:]+)\s*<\/(?:function|methodname)>\s*<\/refpurpose>/i", $xml, $matches);
            $aliasName = $matches[1] ?? null;
            $alias = $funcMap[$aliasName] ?? null;
            $funcName = $matches[2] ?? null;
            $func = $funcMap[$funcName] ?? null;

            if ($alias &&
                !$alias->isUndocumentable &&
                ($func === null || $func->alias === null || $func->alias->__toString() !== $aliasName) &&
                ($alias->alias === null || $alias->alias->__toString() !== $funcName)
            ) {
                $methodSynopsisWarnings[] = "$aliasName()" . ($alias->alias ? " is an alias of " . $alias->alias->__toString() . "(), but it" : "") . " is incorrectly documented as an alias for $funcName()";
            }

            $matches = [];
            preg_match("/<(?:para|simpara)>\s*(?:&info.function.alias;|&info.method.alias;|&Alias;)\s+<(?:function|methodname)>\s*([\w:]+)\s*<\/(?:function|methodname)>/i", $xml, $matches);
            $descriptionFuncName = $matches[1] ?? null;
            $descriptionFunc = $funcMap[$descriptionFuncName] ?? null;
            if ($descriptionFunc && $funcName !== $descriptionFuncName) {
                $methodSynopsisWarnings[] = "Alias in the method synopsis description of $pathName doesn't match the alias in the <refpurpose>";
            }

            if ($aliasName) {
                $documentedFuncMap[$aliasName] = $aliasName;
            }
        }

        if (stripos($xml, "<methodsynopsis") === false && stripos($xml, "<constructorsynopsis") === false && stripos($xml, "<destructorsynopsis") === false) {
            continue;
        }

        $replacedXml = getReplacedSynopsisXml($xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $methodSynopsisElements = [];
        foreach ($doc->getElementsByTagName("constructorsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }
        foreach ($doc->getElementsByTagName("destructorsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }
        foreach ($doc->getElementsByTagName("methodsynopsis") as $element) {
            $methodSynopsisElements[] = $element;
        }

        foreach ($methodSynopsisElements as $methodSynopsis) {
            if (!$methodSynopsis instanceof DOMElement) {
                continue;
            }

            $item = $methodSynopsis->getElementsByTagName("methodname")->item(0);
            if (!$item instanceof DOMElement) {
                continue;
            }
            $funcName = $item->textContent;
            if (!isset($funcMap[$funcName])) {
                continue;
            }

            $funcInfo = $funcMap[$funcName];
            $documentedFuncMap[$funcInfo->name->__toString()] = $funcInfo->name->__toString();

            $newMethodSynopsis = $funcInfo->getMethodSynopsisElement($funcMap, $aliasMap, $doc);
            if ($newMethodSynopsis === null) {
                continue;
            }

            // Retrieve current signature

            $params = [];
            $list = $methodSynopsis->getElementsByTagName("methodparam");
            foreach ($list as $i => $item) {
                if (!$item instanceof DOMElement) {
                    continue;
                }

                $paramList = $item->getElementsByTagName("parameter");
                if ($paramList->count() !== 1) {
                    continue;
                }

                $paramName = $paramList->item(0)->textContent;
                $paramTypes = [];

                $paramList = $item->getElementsByTagName("type");
                foreach ($paramList as $type) {
                    if (!$type instanceof DOMElement) {
                        continue;
                    }

                    $paramTypes[] = $type->textContent;
                }

                $params[$paramName] = ["index" => $i, "type" => $paramTypes];
            }

            // Check if there is any change - short circuit if there is not any.

            if (replaceAndCompareXmls($doc, $methodSynopsis, $newMethodSynopsis)) {
                continue;
            }

            // Update parameter references

            $paramList = $doc->getElementsByTagName("parameter");
            /** @var DOMElement $paramElement */
            foreach ($paramList as $paramElement) {
                if ($paramElement->parentNode && $paramElement->parentNode->nodeName === "methodparam") {
                    continue;
                }

                $name = $paramElement->textContent;
                if (!isset($params[$name])) {
                    continue;
                }

                $index = $params[$name]["index"];
                if (!isset($funcInfo->args[$index])) {
                    continue;
                }

                $paramElement->textContent = $funcInfo->args[$index]->name;
            }

            // Return the updated XML

            $replacedXml = $doc->saveXML();

            $replacedXml = preg_replace(
                [
                    "/REPLACED-ENTITY-([A-Za-z0-9._{}%-]+?;)/",
                    '/<refentry\s+xmlns="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                    '/<refentry\s+xmlns="([^"]+)"\s+xmlns:xlink="([^"]+)"\s+xml:id="([^"]+)"\s*>/i',
                ],
                [
                    "&$1",
                    "<refentry xml:id=\"$2\" xmlns=\"$1\">",
                    "<refentry xml:id=\"$3\" xmlns=\"$1\" xmlns:xlink=\"$2\">",
                ],
                $replacedXml
            );

            $methodSynopses[$pathName] = $replacedXml;
        }
    }

    $undocumentedFuncMap = array_diff_key($funcMap, $documentedFuncMap);

    return $methodSynopses;
}

function replaceAndCompareXmls(DOMDocument $doc, DOMElement $originalSynopsis, DOMElement $newSynopsis): bool
{
    $docComparator = new DOMDocument();
    $docComparator->preserveWhiteSpace = false;
    $docComparator->formatOutput = true;

    $xml1 = $doc->saveXML($originalSynopsis);
    $xml1 = getReplacedSynopsisXml($xml1);
    $docComparator->loadXML($xml1);
    $xml1 = $docComparator->saveXML();

    $originalSynopsis->parentNode->replaceChild($newSynopsis, $originalSynopsis);

    $xml2 = $doc->saveXML($newSynopsis);
    $xml2 = getReplacedSynopsisXml($xml2);

    $docComparator->loadXML($xml2);
    $xml2 = $docComparator->saveXML();

    return $xml1 === $xml2;
}

function installPhpParser(string $version, string $phpParserDir) {
    $lockFile = __DIR__ . "/PHP-Parser-install-lock";
    $lockFd = fopen($lockFile, 'w+');
    if (!flock($lockFd, LOCK_EX)) {
        throw new Exception("Failed to acquire installation lock");
    }

    try {
        // Check whether a parallel process has already installed PHP-Parser.
        if (is_dir($phpParserDir)) {
            return;
        }

        $cwd = getcwd();
        chdir(__DIR__);

        $tarName = "v$version.tar.gz";
        passthru("wget https://github.com/nikic/PHP-Parser/archive/$tarName", $exit);
        if ($exit !== 0) {
            passthru("curl -LO https://github.com/nikic/PHP-Parser/archive/$tarName", $exit);
        }
        if ($exit !== 0) {
            throw new Exception("Failed to download PHP-Parser tarball");
        }
        if (!mkdir($phpParserDir)) {
            throw new Exception("Failed to create directory $phpParserDir");
        }
        passthru("tar xvzf $tarName -C PHP-Parser-$version --strip-components 1", $exit);
        if ($exit !== 0) {
            throw new Exception("Failed to extract PHP-Parser tarball");
        }
        unlink(__DIR__ . "/$tarName");
        chdir($cwd);
    } finally {
        flock($lockFd, LOCK_UN);
        @unlink($lockFile);
    }
}

function initPhpParser() {
    static $isInitialized = false;
    if ($isInitialized) {
        return;
    }

    if (!extension_loaded("tokenizer")) {
        throw new Exception("The \"tokenizer\" extension is not available");
    }

    $isInitialized = true;
    $version = "5.0.0alpha3";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        installPhpParser($version, $phpParserDir);
    }

    spl_autoload_register(static function(string $class) use ($phpParserDir) {
        if (strpos($class, "PhpParser\\") === 0) {
            $fileName = $phpParserDir . "/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}

$optind = null;
$options = getopt(
    "fh",
    [
        "force-regeneration", "parameter-stats", "help", "verify", "verify-manual", "replace-predefined-constants",
        "generate-classsynopses", "replace-classsynopses", "generate-methodsynopses", "replace-methodsynopses",
        "generate-optimizer-info",
    ],
    $optind
);

$context = new Context;
$printParameterStats = isset($options["parameter-stats"]);
$verify = isset($options["verify"]);
$verifyManual = isset($options["verify-manual"]);
$replacePredefinedConstants = isset($options["replace-predefined-constants"]);
$generateClassSynopses = isset($options["generate-classsynopses"]);
$replaceClassSynopses = isset($options["replace-classsynopses"]);
$generateMethodSynopses = isset($options["generate-methodsynopses"]);
$replaceMethodSynopses = isset($options["replace-methodsynopses"]);
$generateOptimizerInfo = isset($options["generate-optimizer-info"]);
$context->forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$context->forceParse = $context->forceRegeneration || $printParameterStats || $verify || $verifyManual || $replacePredefinedConstants || $generateClassSynopses || $generateOptimizerInfo || $replaceClassSynopses || $generateMethodSynopses || $replaceMethodSynopses;

if (isset($options["h"]) || isset($options["help"])) {
    die("\nUsage: gen_stub.php [ -f | --force-regeneration ] [ --replace-predefined-constants ] [ --generate-classsynopses ] [ --replace-classsynopses ] [ --generate-methodsynopses ] [ --replace-methodsynopses ] [ --parameter-stats ] [ --verify ]  [ --verify-manual ] [ --generate-optimizer-info ] [ -h | --help ] [ name.stub.php | directory ] [ directory ]\n\n");
}

$locations = array_slice($argv, $optind);
$locationCount = count($locations);
if ($replacePredefinedConstants && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-predefined-constants ./ ../doc-en/\n");
}
if ($replaceClassSynopses && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-classsynopses ./ ../doc-en/\n");
}
if ($replaceMethodSynopses && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --replace-methodsynopses ./ ../doc-en/\n");
}
if ($verifyManual && $locationCount < 2) {
    die("At least one source stub path and a target manual directory has to be provided:\n./build/gen_stub.php --verify-manual ./ ../doc-en/\n");
}
$manualTarget = null;
if ($replacePredefinedConstants || $replaceClassSynopses || $replaceMethodSynopses || $verifyManual) {
    $manualTarget = array_pop($locations);
}
if ($locations === []) {
    $locations = ['.'];
}

$fileInfos = [];
foreach (array_unique($locations) as $location) {
    if (is_file($location)) {
        // Generate single file.
        $fileInfo = processStubFile($location, $context);
        if ($fileInfo) {
            $fileInfos[] = $fileInfo;
        }
    } else if (is_dir($location)) {
        array_push($fileInfos, ...processDirectory($location, $context));
    } else {
        echo "$location is neither a file nor a directory.\n";
        exit(1);
    }
}

if ($printParameterStats) {
    $parameterStats = [];

    foreach ($fileInfos as $fileInfo) {
        foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
            foreach ($funcInfo->args as $argInfo) {
                if (!isset($parameterStats[$argInfo->name])) {
                    $parameterStats[$argInfo->name] = 0;
                }
                $parameterStats[$argInfo->name]++;
            }
        }
    }

    arsort($parameterStats);
    echo json_encode($parameterStats, JSON_PRETTY_PRINT), "\n";
}

/** @var array<string, ClassInfo> $classMap */
$classMap = [];
/** @var array<string, FuncInfo> $funcMap */
$funcMap = [];
/** @var array<string, FuncInfo> $aliasMap */
$aliasMap = [];

/** @var array<string, ConstInfo> $undocumentedConstMap */
$undocumentedConstMap = [];
/** @var array<string, ClassInfo> $undocumentedClassMap */
$undocumentedClassMap = [];
/** @var array<string, FuncInfo> $undocumentedFuncMap */
$undocumentedFuncMap = [];
/** @var array<int, string> $methodSynopsisWarnings */
$methodSynopsisWarnings = [];

foreach ($fileInfos as $fileInfo) {
    foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
        $funcMap[$funcInfo->name->__toString()] = $funcInfo;

        // TODO: Don't use aliasMap for methodsynopsis?
        if ($funcInfo->aliasType === "alias") {
            $aliasMap[$funcInfo->alias->__toString()] = $funcInfo;
        }
    }

    foreach ($fileInfo->classInfos as $classInfo) {
        $classMap[$classInfo->name->__toString()] = $classInfo;
    }
}

if ($verify) {
    $errors = [];

    foreach ($funcMap as $aliasFunc) {
        if (!$aliasFunc->alias) {
            continue;
        }

        if (!isset($funcMap[$aliasFunc->alias->__toString()])) {
            $errors[] = "Aliased function {$aliasFunc->alias}() cannot be found";
            continue;
        }

        if (!$aliasFunc->verify) {
            continue;
        }

        $aliasedFunc = $funcMap[$aliasFunc->alias->__toString()];
        $aliasedArgs = $aliasedFunc->args;
        $aliasArgs = $aliasFunc->args;

        if ($aliasFunc->isInstanceMethod() !== $aliasedFunc->isInstanceMethod()) {
            if ($aliasFunc->isInstanceMethod()) {
                $aliasedArgs = array_slice($aliasedArgs, 1);
            }

            if ($aliasedFunc->isInstanceMethod()) {
                $aliasArgs = array_slice($aliasArgs, 1);
            }
        }

        array_map(
            function(?ArgInfo $aliasArg, ?ArgInfo $aliasedArg) use ($aliasFunc, $aliasedFunc, &$errors) {
                if ($aliasArg === null) {
                    assert($aliasedArg !== null);
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() is missing";
                    return null;
                }

                if ($aliasedArg === null) {
                    $errors[] = "{$aliasedFunc->name}(): Argument \$$aliasArg->name of alias function {$aliasFunc->name}() is missing";
                    return null;
                }

                if ($aliasArg->name !== $aliasedArg->name) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same name";
                    return null;
                }

                if ($aliasArg->type != $aliasedArg->type) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same type";
                }

                if ($aliasArg->defaultValue !== $aliasedArg->defaultValue) {
                    $errors[] = "{$aliasFunc->name}(): Argument \$$aliasArg->name and argument \$$aliasedArg->name of aliased function {$aliasedFunc->name}() must have the same default value";
                }
            },
            $aliasArgs, $aliasedArgs
        );

        $aliasedReturn = $aliasedFunc->return;
        $aliasReturn = $aliasFunc->return;

        if (!$aliasedFunc->name->isConstructor() && !$aliasFunc->name->isConstructor()) {
            $aliasedReturnType = $aliasedReturn->type ?? $aliasedReturn->phpDocType;
            $aliasReturnType = $aliasReturn->type ?? $aliasReturn->phpDocType;
            if ($aliasReturnType != $aliasedReturnType) {
                $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same return type";
            }
        }

        $aliasedPhpDocReturnType = $aliasedReturn->phpDocType;
        $aliasPhpDocReturnType = $aliasReturn->phpDocType;
        if ($aliasedPhpDocReturnType != $aliasPhpDocReturnType && $aliasedPhpDocReturnType != $aliasReturn->type && $aliasPhpDocReturnType != $aliasedReturn->type) {
            $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same PHPDoc return type";
        }
    }

    echo implode("\n", $errors);
    if (!empty($errors)) {
        echo "\n";
        exit(1);
    }
}

if ($replacePredefinedConstants || $verifyManual) {
    $predefinedConstants = replacePredefinedConstants($manualTarget, $context->allConstInfos, $undocumentedConstMap);

    if ($replacePredefinedConstants) {
        foreach ($predefinedConstants as $filename => $content) {
            if (file_put_contents($filename, $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($generateClassSynopses) {
    $classSynopsesDirectory = getcwd() . "/classsynopses";

    $classSynopses = generateClassSynopses($classMap, $context->allConstInfos);
    if (!empty($classSynopses)) {
        if (!file_exists($classSynopsesDirectory)) {
            mkdir($classSynopsesDirectory);
        }

        foreach ($classSynopses as $filename => $content) {
            if (file_put_contents("$classSynopsesDirectory/$filename", $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($replaceClassSynopses || $verifyManual) {
    $classSynopses = replaceClassSynopses($manualTarget, $classMap, $context->allConstInfos, $undocumentedClassMap);

    if ($replaceClassSynopses) {
        foreach ($classSynopses as $filename => $content) {
            if (file_put_contents($filename, $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($generateMethodSynopses) {
    // Use the manual as target if we are targeting a specific extension
    if (str_contains($manualTarget, 'reference')) {
        $methodSynopsesDirectory = $manualTarget;
    } else {
        $methodSynopsesDirectory = getcwd() . "/methodsynopses";
    }

    $methodSynopses = generateMethodSynopses($funcMap, $aliasMap);
    if (!empty($methodSynopses)) {
        if (!file_exists($methodSynopsesDirectory)) {
            mkdir($methodSynopsesDirectory);
        }

        foreach ($methodSynopses as $filename => $content) {
            if (!file_exists("$methodSynopsesDirectory/$filename")) {
                if (file_put_contents("$methodSynopsesDirectory/$filename", $content)) {
                    echo "Saved $filename\n";
                }
            }
        }
    }
}

if ($replaceMethodSynopses || $verifyManual) {
    $methodSynopses = replaceMethodSynopses($manualTarget, $funcMap, $aliasMap, $verifyManual, $methodSynopsisWarnings, $undocumentedFuncMap);

    if ($replaceMethodSynopses) {
        foreach ($methodSynopses as $filename => $content) {
            if (file_put_contents($filename, $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($generateOptimizerInfo) {
    $filename = dirname(__FILE__, 2) . "/Zend/Optimizer/zend_func_infos.h";
    $optimizerInfo = generateOptimizerInfo($funcMap);

    if (file_put_contents($filename, $optimizerInfo)) {
        echo "Saved $filename\n";
    }
}

if ($verifyManual) {
    foreach ($undocumentedConstMap as $constName => $info) {
        if ($info->name->isClassConst() || $info->isUndocumentable) {
            continue;
        }

        echo "Warning: Missing predefined constant for $constName\n";
    }

    foreach ($methodSynopsisWarnings as $warning) {
        echo "Warning: $warning\n";
    }

    foreach ($undocumentedClassMap as $className => $info) {
        if (!$info->isUndocumentable) {
            echo "Warning: Missing class synopsis for $className\n";
        }
    }

    foreach ($undocumentedFuncMap as $functionName => $info) {
        if (!$info->isUndocumentable) {
            echo "Warning: Missing method synopsis for $functionName()\n";
        }
    }
}
