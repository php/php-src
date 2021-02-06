#!/usr/bin/env php
<?php declare(strict_types=1);

use PhpParser\Comment\Doc as DocComment;
use PhpParser\Node;
use PhpParser\Node\Expr;
use PhpParser\Node\Name;
use PhpParser\Node\Stmt;
use PhpParser\Node\Stmt\Class_;
use PhpParser\PrettyPrinter\Standard;
use PhpParser\PrettyPrinterAbstract;

error_reporting(E_ALL);

/**
 * @return FileInfo[]
 */
function processDirectory(string $dir, Context $context): array {
    $fileInfos = [];

    $it = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir),
        RecursiveIteratorIterator::LEAVES_ONLY
    );
    foreach ($it as $file) {
        $pathName = $file->getPathName();
        if (preg_match('/\.stub\.php$/', $pathName)) {
            $fileInfo = processStubFile($pathName, $context);
            if ($fileInfo) {
                $fileInfos[] = $fileInfo;
            }
        }
    }

    return $fileInfos;
}

function processStubFile(string $stubFile, Context $context): ?FileInfo {
    try {
        if (!file_exists($stubFile)) {
            throw new Exception("File $stubFile does not exist");
        }

        $arginfoFile = str_replace('.stub.php', '_arginfo.h', $stubFile);
        $legacyFile = str_replace('.stub.php', '_legacy_arginfo.h', $stubFile);

        $stubCode = file_get_contents($stubFile);
        $stubHash = computeStubHash($stubCode);
        $oldStubHash = extractStubHash($arginfoFile);
        if ($stubHash === $oldStubHash && !$context->forceParse) {
            /* Stub file did not change, do not regenerate. */
            return null;
        }

        initPhpParser();
        $fileInfo = parseStubFile($stubCode);
        $arginfoCode = generateArgInfoCode($fileInfo, $stubHash);
        if (($context->forceRegeneration || $stubHash !== $oldStubHash) && file_put_contents($arginfoFile, $arginfoCode)) {
            echo "Saved $arginfoFile\n";
        }

        if ($fileInfo->generateLegacyArginfo) {
            foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
                $funcInfo->discardInfoForOldPhpVersions();
            }
            $arginfoCode = generateArgInfoCode($fileInfo, $stubHash);
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
    /** @var bool */
    public $forceParse = false;
    /** @var bool */
    public $forceRegeneration = false;
}

class SimpleType {
    /** @var string */
    public $name;
    /** @var bool */
    public $isBuiltin;

    public function __construct(string $name, bool $isBuiltin) {
        $this->name = $name;
        $this->isBuiltin = $isBuiltin;
    }

    public static function fromNode(Node $node): SimpleType {
        if ($node instanceof Node\Name) {
            if ($node->toLowerString() === 'static') {
                // PHP internally considers "static" a builtin type.
                return new SimpleType($node->toString(), true);
            }

            assert($node->isFullyQualified());
            return new SimpleType($node->toString(), false);
        }
        if ($node instanceof Node\Identifier) {
            return new SimpleType($node->toString(), true);
        }
        throw new Exception("Unexpected node type");
    }

    public static function fromPhpDoc(string $type): SimpleType
    {
        switch (strtolower($type)) {
            case "void":
            case "null":
            case "false":
            case "bool":
            case "int":
            case "float":
            case "string":
            case "array":
            case "iterable":
            case "object":
            case "resource":
            case "mixed":
            case "self":
            case "static":
                return new SimpleType(strtolower($type), true);
        }

        if (strpos($type, "[]") !== false) {
            return new SimpleType("array", true);
        }

        return new SimpleType($type, false);
    }

    public static function null(): SimpleType
    {
        return new SimpleType("null", true);
    }

    public static function void(): SimpleType
    {
        return new SimpleType("void", true);
    }

    public function isNull(): bool {
        return $this->isBuiltin && $this->name === 'null';
    }

    public function toTypeCode(): string {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
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
        case "iterable":
            return "IS_ITERABLE";
        case "mixed":
            return "IS_MIXED";
        case "static":
            return "IS_STATIC";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toTypeMask() {
        assert($this->isBuiltin);
        switch (strtolower($this->name)) {
        case "null":
            return "MAY_BE_NULL";
        case "false":
            return "MAY_BE_FALSE";
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
        case "static":
            return "MAY_BE_STATIC";
        default:
            throw new Exception("Not implemented: $this->name");
        }
    }

    public function toEscapedName(): string {
        return str_replace('\\', '\\\\', $this->name);
    }

    public function equals(SimpleType $other) {
        return $this->name === $other->name
            && $this->isBuiltin === $other->isBuiltin;
    }
}

class Type {
    /** @var SimpleType[] $types */
    public $types;

    public function __construct(array $types) {
        $this->types = $types;
    }

    public static function fromNode(Node $node): Type {
        if ($node instanceof Node\UnionType) {
            return new Type(array_map(['SimpleType', 'fromNode'], $node->types));
        }
        if ($node instanceof Node\NullableType) {
            return new Type([
                SimpleType::fromNode($node->type),
                SimpleType::null(),
            ]);
        }
        return new Type([SimpleType::fromNode($node)]);
    }

    public static function fromPhpDoc(string $phpDocType) {
        $types = explode("|", $phpDocType);

        $simpleTypes = [];
        foreach ($types as $type) {
            $simpleTypes[] = SimpleType::fromPhpDoc($type);
        }

        return new Type($simpleTypes);
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
        return new Type(array_filter($this->types, function(SimpleType $type) {
            return !$type->isNull();
        }));
    }

    public function tryToSimpleType(): ?SimpleType {
        $withoutNull = $this->getWithoutNull();
        if (count($withoutNull->types) === 1) {
            return $withoutNull->types[0];
        }
        return null;
    }

    public function toArginfoType(): ?ArginfoType {
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

        return implode('|', array_map(
            function ($type) { return $type->name; },
            $this->types)
        );
    }
}

class ArginfoType {
    /** @var ClassType[] $classTypes */
    public $classTypes;

    /** @var SimpleType[] $builtinTypes */
    private $builtinTypes;

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

    /** @var string */
    public $name;
    /** @var int */
    public $sendBy;
    /** @var bool */
    public $isVariadic;
    /** @var Type|null */
    public $type;
    /** @var Type|null */
    public $phpDocType;
    /** @var string|null */
    public $defaultValue;

    public function __construct(string $name, int $sendBy, bool $isVariadic, ?Type $type, ?Type $phpDocType, ?string $defaultValue) {
        $this->name = $name;
        $this->sendBy = $sendBy;
        $this->isVariadic = $isVariadic;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
        $this->defaultValue = $defaultValue;
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
}

interface FunctionOrMethodName {
    public function getDeclaration(): string;
    public function getArgInfoName(): string;
    public function getMethodSynopsisFilename(): string;
    public function __toString(): string;
    public function isMethod(): bool;
    public function isConstructor(): bool;
    public function isDestructor(): bool;
}

class FunctionName implements FunctionOrMethodName {
    /** @var Name */
    private $name;

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
        return $this->name->getLast();
    }

    public function getDeclaration(): string {
        return "ZEND_FUNCTION({$this->getDeclarationName()});\n";
    }

    public function getArgInfoName(): string {
        $underscoreName = implode('_', $this->name->parts);
        return "arginfo_$underscoreName";
    }

    public function getMethodSynopsisFilename(): string {
        return implode('_', $this->name->parts);
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
    /** @var Name */
    private $className;
    /** @var string */
    public $methodName;

    public function __construct(Name $className, string $methodName) {
        $this->className = $className;
        $this->methodName = $methodName;
    }

    public function getDeclarationClassName(): string {
        return implode('_', $this->className->parts);
    }

    public function getDeclaration(): string {
        return "ZEND_METHOD({$this->getDeclarationClassName()}, $this->methodName);\n";
    }

    public function getArgInfoName(): string {
        return "arginfo_class_{$this->getDeclarationClassName()}_{$this->methodName}";
    }

    public function getMethodSynopsisFilename(): string {
        return $this->getDeclarationClassName() . "_{$this->methodName}";
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
    /** @var bool */
    public $byRef;
    /** @var Type|null */
    public $type;
    /** @var Type|null */
    public $phpDocType;

    public function __construct(bool $byRef, ?Type $type, ?Type $phpDocType) {
        $this->byRef = $byRef;
        $this->type = $type;
        $this->phpDocType = $phpDocType;
    }

    public function equals(ReturnInfo $other): bool {
        return $this->byRef === $other->byRef
            && Type::equals($this->type, $other->type);
    }

    public function getMethodSynopsisType(): ?Type {
        return $this->type ?? $this->phpDocType;
    }
}

class FuncInfo {
    /** @var FunctionOrMethodName */
    public $name;
    /** @var int */
    public $classFlags;
    /** @var int */
    public $flags;
    /** @var string|null */
    public $aliasType;
    /** @var FunctionName|null */
    public $alias;
    /** @var bool */
    public $isDeprecated;
    /** @var bool */
    public $verify;
    /** @var ArgInfo[] */
    public $args;
    /** @var ReturnInfo */
    public $return;
    /** @var int */
    public $numRequiredArgs;
    /** @var string|null */
    public $cond;

    public function __construct(
        FunctionOrMethodName $name,
        int $classFlags,
        int $flags,
        ?string $aliasType,
        ?FunctionOrMethodName $alias,
        bool $isDeprecated,
        bool $verify,
        array $args,
        ReturnInfo $return,
        int $numRequiredArgs,
        ?string $cond
    ) {
        $this->name = $name;
        $this->classFlags = $classFlags;
        $this->flags = $flags;
        $this->aliasType = $aliasType;
        $this->alias = $alias;
        $this->isDeprecated = $isDeprecated;
        $this->verify = $verify;
        $this->args = $args;
        $this->return = $return;
        $this->numRequiredArgs = $numRequiredArgs;
        $this->cond = $cond;
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

    public function equalsApartFromName(FuncInfo $other): bool {
        if (count($this->args) !== count($other->args)) {
            return false;
        }

        for ($i = 0; $i < count($this->args); $i++) {
            if (!$this->args[$i]->equals($other->args[$i])) {
                return false;
            }
        }

        return $this->return->equals($other->return)
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
            $declarationName = $this->name->getDeclarationName();

            if ($this->alias && $this->isDeprecated) {
                return sprintf(
                    "\tZEND_DEP_FALIAS(%s, %s, %s)\n",
                    $declarationName, $this->alias->getNonNamespacedName(), $this->getArgInfoName()
                );
            }

            if ($this->alias) {
                return sprintf(
                    "\tZEND_FALIAS(%s, %s, %s)\n",
                    $declarationName, $this->alias->getNonNamespacedName(), $this->getArgInfoName()
                );
            }

            if ($this->isDeprecated) {
                return sprintf(
                    "\tZEND_DEP_FE(%s, %s)\n", $declarationName, $this->getArgInfoName());
            }

            if ($namespace) {
                // Render A\B as "A\\B" in C strings for namespaces
                return sprintf(
                    "\tZEND_NS_FE(\"%s\", %s, %s)\n",
                    addslashes($namespace), $declarationName, $this->getArgInfoName());
            } else {
                return sprintf("\tZEND_FE(%s, %s)\n", $declarationName, $this->getArgInfoName());
            }
        } else {
            throw new Error("Cannot happen");
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

    /**
     * @param FuncInfo[] $funcMap
     * @param FuncInfo[] $aliasMap
     * @throws Exception
     */
    public function getMethodSynopsisDocument(array $funcMap, array $aliasMap): ?string {

        $doc = new DOMDocument();
        $doc->formatOutput = true;
        $methodSynopsis = $this->getMethodSynopsisElement($funcMap, $aliasMap, $doc);
        if (!$methodSynopsis) {
            return null;
        }

        $doc->appendChild($methodSynopsis);

        return $doc->saveXML();
    }

    /**
     * @param FuncInfo[] $funcMap
     * @param FuncInfo[] $aliasMap
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

        $aliasedFunc = $this->aliasType === "alias" && isset($funcMap[$this->alias->__toString()]) ? $funcMap[$this->alias->__toString()] : null;
        $aliasFunc = $aliasMap[$this->name->__toString()] ?? null;

        if (($this->aliasType === "alias" && $aliasedFunc !== null && $aliasedFunc->isMethod() !== $this->isMethod()) ||
            ($aliasFunc !== null && $aliasFunc->isMethod() !== $this->isMethod())
        ) {
            $role = $doc->createAttribute("role");
            $role->value = $this->isMethod() ? "oop" : "procedural";
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
            $this->appendMethodSynopsisTypeToElement($doc, $methodSynopsis, $returnType);
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
                $this->appendMethodSynopsisTypeToElement($doc, $methodparam, $arg->getMethodSynopsisType());

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

    public function discardInfoForOldPhpVersions(): void {
        $this->return->type = null;
        foreach ($this->args as $arg) {
            $arg->type = null;
            $arg->defaultValue = null;
        }
    }

    private function appendMethodSynopsisTypeToElement(DOMDocument $doc, DOMElement $elementToAppend, Type $type) {
        if (count($type->types) > 1) {
            $typeElement = $doc->createElement('type');
            $typeElement->setAttribute("class", "union");

            foreach ($type->types as $type) {
                $unionTypeElement = $doc->createElement('type', $type->name);
                $typeElement->appendChild($unionTypeElement);
            }
        } else {
            $typeElement = $doc->createElement('type', $type->types[0]->name);
        }

        $elementToAppend->appendChild($typeElement);
    }
}

class ClassInfo {
    /** @var Name */
    public $name;
    /** @var FuncInfo[] */
    public $funcInfos;

    public function __construct(Name $name, array $funcInfos) {
        $this->name = $name;
        $this->funcInfos = $funcInfos;
    }
}

class FileInfo {
    /** @var FuncInfo[] */
    public $funcInfos = [];
    /** @var ClassInfo[] */
    public $classInfos = [];
    /** @var bool */
    public $generateFunctionEntries = false;
    /** @var string */
    public $declarationPrefix = "";
    /** @var bool */
    public $generateLegacyArginfo = false;

    /**
     * @return iterable<FuncInfo>
     */
    public function getAllFuncInfos(): iterable {
        yield from $this->funcInfos;
        foreach ($this->classInfos as $classInfo) {
            yield from $classInfo->funcInfos;
        }
    }
}

class DocCommentTag {
    /** @var string */
    public $name;
    /** @var string|null */
    public $value;

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
            preg_match('/^\s*([\w\|\\\\\[\]]+)\s*\$\w+.*$/', $value, $matches);
        } elseif ($this->name === "return") {
            preg_match('/^\s*([\w\|\\\\\[\]]+)\s*$/', $value, $matches);
        }

        if (isset($matches[1]) === false) {
            throw new Exception("@$this->name doesn't contain a type or has an invalid format \"$value\"");
        }

        return $matches[1];
    }

    public function getVariableName(): string {
        $value = $this->value;
        if ($value === null || strlen($value) === 0) {
            throw new Exception("@$this->name doesn't have any value");
        }

        $matches = [];

        if ($this->name === "param") {
            preg_match('/^\s*[\w\|\\\\\[\]]+\s*\$(\w+).*$/', $value, $matches);
        } elseif ($this->name === "prefer-ref") {
            preg_match('/^\s*\$(\w+).*$/', $value, $matches);
        }

        if (isset($matches[1]) === false) {
            throw new Exception("@$this->name doesn't contain a variable name or has an invalid format \"$value\"");
        }

        return $matches[1];
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
    ?string $cond
): FuncInfo {
    $comment = $func->getDocComment();
    $paramMeta = [];
    $aliasType = null;
    $alias = null;
    $isDeprecated = false;
    $verify = true;
    $docReturnType = null;
    $docParamTypes = [];

    if ($comment) {
        $tags = parseDocComment($comment);
        foreach ($tags as $tag) {
            if ($tag->name === 'prefer-ref') {
                $varName = $tag->getVariableName();
                if (!isset($paramMeta[$varName])) {
                    $paramMeta[$varName] = [];
                }
                $paramMeta[$varName]['preferRef'] = true;
            } else if ($tag->name === 'alias' || $tag->name === 'implementation-alias') {
                $aliasType = $tag->name;
                $aliasParts = explode("::", $tag->getValue());
                if (count($aliasParts) === 1) {
                    $alias = new FunctionName(new Name($aliasParts[0]));
                } else {
                    $alias = new MethodName(new Name($aliasParts[0]), $aliasParts[1]);
                }
            } else if ($tag->name === 'deprecated') {
                $isDeprecated = true;
            }  else if ($tag->name === 'no-verify') {
                $verify = false;
            } else if ($tag->name === 'return') {
                $docReturnType = $tag->getType();
            } else if ($tag->name === 'param') {
                $docParamTypes[$tag->getVariableName()] = $tag->getType();
            }
        }
    }

    $varNameSet = [];
    $args = [];
    $numRequiredArgs = 0;
    $foundVariadic = false;
    foreach ($func->getParams() as $i => $param) {
        $varName = $param->var->name;
        $preferRef = !empty($paramMeta[$varName]['preferRef']);
        unset($paramMeta[$varName]);

        if (isset($varNameSet[$varName])) {
            throw new Exception("Duplicate parameter name $varName for function $name");
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
            throw new Exception("Error in function $name: only the last parameter can be variadic");
        }

        $type = $param->type ? Type::fromNode($param->type) : null;
        if ($type === null && !isset($docParamTypes[$varName])) {
            throw new Exception("Missing parameter type for function $name()");
        }

        if ($param->default instanceof Expr\ConstFetch &&
            $param->default->name->toLowerString() === "null" &&
            $type && !$type->isNullable()
        ) {
            $simpleType = $type->tryToSimpleType();
            if ($simpleType === null) {
                throw new Exception(
                    "Parameter $varName of function $name has null default, but is not nullable");
            }
        }

        $foundVariadic = $param->variadic;

        $args[] = new ArgInfo(
            $varName,
            $sendBy,
            $param->variadic,
            $type,
            isset($docParamTypes[$varName]) ? Type::fromPhpDoc($docParamTypes[$varName]) : null,
            $param->default ? $prettyPrinter->prettyPrintExpr($param->default) : null
        );
        if (!$param->default && !$param->variadic) {
            $numRequiredArgs = $i + 1;
        }
    }

    foreach (array_keys($paramMeta) as $var) {
        throw new Exception("Found metadata for invalid param $var of function $name");
    }

    $returnType = $func->getReturnType();
    if ($returnType === null && $docReturnType === null && !$name->isConstructor() && !$name->isDestructor()) {
        throw new Exception("Missing return type for function $name()");
    }

    $return = new ReturnInfo(
        $func->returnsByRef(),
        $returnType ? Type::fromNode($returnType) : null,
        $docReturnType ? Type::fromPhpDoc($docReturnType) : null
    );

    return new FuncInfo(
        $name,
        $classFlags,
        $flags,
        $aliasType,
        $alias,
        $isDeprecated,
        $verify,
        $args,
        $return,
        $numRequiredArgs,
        $cond
    );
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
        if ($stmt instanceof Stmt\Nop) {
            continue;
        }

        if ($stmt instanceof Stmt\Namespace_) {
            handleStatements($fileInfo, $stmt->stmts, $prettyPrinter);
            continue;
        }

        $cond = handlePreprocessorConditions($conds, $stmt);
        if ($stmt instanceof Stmt\Function_) {
            $fileInfo->funcInfos[] = parseFunctionLike(
                $prettyPrinter,
                new FunctionName($stmt->namespacedName),
                0,
                0,
                $stmt,
                $cond
            );
            continue;
        }

        if ($stmt instanceof Stmt\ClassLike) {
            $className = $stmt->namespacedName;
            $methodInfos = [];
            foreach ($stmt->stmts as $classStmt) {
                $cond = handlePreprocessorConditions($conds, $classStmt);
                if ($classStmt instanceof Stmt\Nop) {
                    continue;
                }

                if (!$classStmt instanceof Stmt\ClassMethod) {
                    throw new Exception("Not implemented {$classStmt->getType()}");
                }

                $classFlags = 0;
                if ($stmt instanceof Class_) {
                    $classFlags = $stmt->flags;
                }

                $flags = $classStmt->flags;
                if ($stmt instanceof Stmt\Interface_) {
                    $flags |= Class_::MODIFIER_ABSTRACT;
                }

                if (!($flags & Class_::VISIBILITY_MODIFIER_MASK)) {
                    throw new Exception("Method visibility modifier is required");
                }

                $methodInfos[] = parseFunctionLike(
                    $prettyPrinter,
                    new MethodName($className, $classStmt->name->toString()),
                    $classFlags,
                    $flags,
                    $classStmt,
                    $cond
                );
            }

            $fileInfo->classInfos[] = new ClassInfo($className, $methodInfos);
            continue;
        }

        throw new Exception("Unexpected node {$stmt->getType()}");
    }
}

function parseStubFile(string $code): FileInfo {
    $lexer = new PhpParser\Lexer();
    $parser = new PhpParser\Parser\Php7($lexer);
    $nodeTraverser = new PhpParser\NodeTraverser;
    $nodeTraverser->addVisitor(new PhpParser\NodeVisitor\NameResolver);
    $prettyPrinter = new class extends Standard {
        protected function pName_FullyQualified(Name\FullyQualified $node) {
            return implode('\\', $node->parts);
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
                $fileInfo->generateLegacyArginfo = true;
            }
        }
    }

    handleStatements($fileInfo, $stmts, $prettyPrinter);
    return $fileInfo;
}

function funcInfoToCode(FuncInfo $funcInfo): string {
    $code = '';
    $returnType = $funcInfo->return->type;
    if ($returnType !== null) {
        if (null !== $simpleReturnType = $returnType->tryToSimpleType()) {
            if ($simpleReturnType->isBuiltin) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toTypeCode(), $returnType->isNullable()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(%s, %d, %d, %s, %d)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $simpleReturnType->toEscapedName(), $returnType->isNullable()
                );
            }
        } else {
            $arginfoType = $returnType->toArginfoType();
            if ($arginfoType->hasClassType()) {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(%s, %d, %d, %s, %s)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toClassTypeString(), $arginfoType->toTypeMask()
                );
            } else {
                $code .= sprintf(
                    "ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(%s, %d, %d, %s)\n",
                    $funcInfo->getArgInfoName(), $funcInfo->return->byRef,
                    $funcInfo->numRequiredArgs,
                    $arginfoType->toTypeMask()
                );
            }
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
                        "\tZEND_%s_OBJ_TYPE_MASK(%s, %s, %s, %s, %s)\n",
                        $argKind, $argInfo->getSendByString(), $argInfo->name,
                        $arginfoType->toClassTypeString(), $arginfoType->toTypeMask(),
                        $argInfo->getDefaultValueAsArginfoString()
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
        if ($generatedFuncInfo->equalsApartFromName($funcInfo)) {
            return $generatedFuncInfo;
        }
    }
    return null;
}

/** @param iterable<FuncInfo> $funcInfos */
function generateCodeWithConditions(
        iterable $funcInfos, string $separator, Closure $codeGenerator): string {
    $code = "";
    foreach ($funcInfos as $funcInfo) {
        $funcCode = $codeGenerator($funcInfo);
        if ($funcCode === null) {
            continue;
        }

        $code .= $separator;
        if ($funcInfo->cond) {
            $code .= "#if {$funcInfo->cond}\n";
            $code .= $funcCode;
            $code .= "#endif\n";
        } else {
            $code .= $funcCode;
        }
    }
    return $code;
}

function generateArgInfoCode(FileInfo $fileInfo, string $stubHash): string {
    $code = "/* This is a generated file, edit the .stub.php file instead.\n"
          . " * Stub hash: $stubHash */\n";
    $generatedFuncInfos = [];
    $code .= generateCodeWithConditions(
        $fileInfo->getAllFuncInfos(), "\n",
        function (FuncInfo $funcInfo) use(&$generatedFuncInfos) {
            /* If there already is an equivalent arginfo structure, only emit a #define */
            if ($generatedFuncInfo = findEquivalentFuncInfo($generatedFuncInfos, $funcInfo)) {
                $code = sprintf(
                    "#define %s %s\n",
                    $funcInfo->getArgInfoName(), $generatedFuncInfo->getArgInfoName()
                );
            } else {
                $code = funcInfoToCode($funcInfo);
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
            function (FuncInfo $funcInfo) use($fileInfo, &$generatedFunctionDeclarations) {
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
            $code .= generateFunctionEntries($classInfo->name, $classInfo->funcInfos);
        }
    }

    return $code;
}

/** @param FuncInfo[] $funcInfos */
function generateFunctionEntries(?Name $className, array $funcInfos): string {
    $code = "";

    $functionEntryName = "ext_functions";
    if ($className) {
        $underscoreName = implode("_", $className->parts);
        $functionEntryName = "class_{$underscoreName}_methods";
    }

    $code .= "\n\nstatic const zend_function_entry {$functionEntryName}[] = {\n";
    $code .= generateCodeWithConditions($funcInfos, "", function (FuncInfo $funcInfo) {
        return $funcInfo->getFunctionEntry();
    });
    $code .= "\tZEND_FE_END\n";
    $code .= "};\n";

    return $code;
}

/**
 * @param FuncInfo[] $funcMap
 * @param FuncInfo[] $aliasMap
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
 * @param FuncInfo[] $funcMap
 * @param FuncInfo[] $aliasMap
 * @return array<string, string>
 */
function replaceMethodSynopses(string $targetDirectory, array $funcMap, array $aliasMap): array {
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

        if (stripos($xml, "<methodsynopsis") === false && stripos($xml, "<constructorsynopsis") === false && stripos($xml, "<destructorsynopsis") === false) {
            continue;
        }

        $replacedXml = preg_replace("/&([A-Za-z0-9._{}%-]+?;)/", "REPLACED-ENTITY-$1", $xml);

        $doc = new DOMDocument();
        $doc->formatOutput = false;
        $doc->preserveWhiteSpace = true;
        $doc->validateOnParse = true;
        $success = $doc->loadXML($replacedXml);
        if (!$success) {
            echo "Failed opening $pathName\n";
            continue;
        }

        $docComparator = new DOMDocument();
        $docComparator->preserveWhiteSpace = false;
        $docComparator->formatOutput = true;

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

            $list = $methodSynopsis->getElementsByTagName("methodname");
            $item = $list->item(0);
            if (!$item instanceof DOMElement) {
                continue;
            }
            $funcName = $item->textContent;
            if (!isset($funcMap[$funcName])) {
                continue;
            }
            $funcInfo = $funcMap[$funcName];

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

            $xml1 = $doc->saveXML($methodSynopsis);
            $xml1 = preg_replace("/&([A-Za-z0-9._{}%-]+?;)/", "REPLACED-ENTITY-$1", $xml1);
            $docComparator->loadXML($xml1);
            $xml1 = $docComparator->saveXML();

            $methodSynopsis->parentNode->replaceChild($newMethodSynopsis, $methodSynopsis);

            $xml2 = $doc->saveXML($newMethodSynopsis);
            $xml2 = preg_replace("/&([A-Za-z0-9._{}%-]+?;)/", "REPLACED-ENTITY-$1", $xml2);
            $docComparator->loadXML($xml2);
            $xml2 = $docComparator->saveXML();

            if ($xml1 === $xml2) {
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
                    "/<refentry\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
                    "/<refentry\s+xmlns=\"([a-z0-9.:\/]+)\"\s+xmlns:xlink=\"([a-z0-9.:\/]+)\"\s+xml:id=\"([a-z0-9._-]+)\"\s*>/i",
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

    return $methodSynopses;
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
    $version = "4.9.0";
    $phpParserDir = __DIR__ . "/PHP-Parser-$version";
    if (!is_dir($phpParserDir)) {
        installPhpParser($version, $phpParserDir);
    }

    spl_autoload_register(function(string $class) use($phpParserDir) {
        if (strpos($class, "PhpParser\\") === 0) {
            $fileName = $phpParserDir . "/lib/" . str_replace("\\", "/", $class) . ".php";
            require $fileName;
        }
    });
}

$optind = null;
$options = getopt("fh", ["force-regeneration", "parameter-stats", "help", "verify", "generate-methodsynopses", "replace-methodsynopses"], $optind);

$context = new Context;
$printParameterStats = isset($options["parameter-stats"]);
$verify = isset($options["verify"]);
$generateMethodSynopses = isset($options["generate-methodsynopses"]);
$replaceMethodSynopses = isset($options["replace-methodsynopses"]);
$context->forceRegeneration = isset($options["f"]) || isset($options["force-regeneration"]);
$context->forceParse = $context->forceRegeneration || $printParameterStats || $verify || $generateMethodSynopses || $replaceMethodSynopses;
$targetMethodSynopses = $argv[$optind + 1] ?? null;
if ($replaceMethodSynopses && $targetMethodSynopses === null) {
    die("A target directory must be provided.\n");
}

if (isset($options["h"]) || isset($options["help"])) {
    die("\nusage: gen-stub.php [ -f | --force-regeneration ] [ --generate-methodsynopses ] [ --replace-methodsynopses ] [ --parameter-stats ] [ --verify ] [ -h | --help ] [ name.stub.php | directory ] [ directory ]\n\n");
}

$fileInfos = [];
$location = $argv[$optind] ?? ".";
if (is_file($location)) {
    // Generate single file.
    $fileInfo = processStubFile($location, $context);
    if ($fileInfo) {
        $fileInfos[] = $fileInfo;
    }
} else if (is_dir($location)) {
    $fileInfos = processDirectory($location, $context);
} else {
    echo "$location is neither a file nor a directory.\n";
    exit(1);
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

/** @var FuncInfo[] $funcMap */
$funcMap = [];
/** @var FuncInfo[] $aliasMap */
$aliasMap = [];

foreach ($fileInfos as $fileInfo) {
    foreach ($fileInfo->getAllFuncInfos() as $funcInfo) {
        /** @var FuncInfo $funcInfo */
        $funcMap[$funcInfo->name->__toString()] = $funcInfo;

        if ($funcInfo->aliasType === "alias") {
            $aliasMap[$funcInfo->alias->__toString()] = $funcInfo;
        }
    }
}

if ($verify) {
    $errors = [];

    foreach ($aliasMap as $aliasFunc) {
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

        if ((!$aliasedFunc->isMethod() || $aliasedFunc->isFinalMethod()) &&
            (!$aliasFunc->isMethod() || $aliasFunc->isFinalMethod()) &&
            $aliasFunc->return != $aliasedFunc->return
        ) {
            $errors[] = "{$aliasFunc->name}() and {$aliasedFunc->name}() must have the same return type";
        }
    }

    echo implode("\n", $errors);
    if (!empty($errors)) {
        echo "\n";
        exit(1);
    }
}

if ($generateMethodSynopses) {
    $methodSynopsesDirectory = getcwd() . "/methodsynopses";

    $methodSynopses = generateMethodSynopses($funcMap, $aliasMap);
    if (!empty($methodSynopses)) {
        if (!file_exists($methodSynopsesDirectory)) {
            mkdir($methodSynopsesDirectory);
        }

        foreach ($methodSynopses as $filename => $content) {
            if (file_put_contents("$methodSynopsesDirectory/$filename", $content)) {
                echo "Saved $filename\n";
            }
        }
    }
}

if ($replaceMethodSynopses) {
    $methodSynopses = replaceMethodSynopses($targetMethodSynopses, $funcMap, $aliasMap);

    foreach ($methodSynopses as $filename => $content) {
        if (file_put_contents($filename, $content)) {
            echo "Saved $filename\n";
        }
    }
}
